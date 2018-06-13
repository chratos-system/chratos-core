// Copyright (c) 2018 The Chratos Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "dividend/dividend.h"
#include "dividend/dividenddb.h"
#include "dividend/dividendtx.h"

#include "ui_interface.h"
#include "main.h"
#include "script/ismine.h"
#include "base58.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>

const char *DEFAULT_DIVIDEND_LEDGER_DAT = "dividendledger.dat";
const char *DIVIDEND_DEFAULT_ADDRESS = "cDIVIDENDADDRESS12345";

CDividendLedger::CDividendLedger() {
  fFileBacked = false;
  nTimeFirstKey = 0;
  Init();
}

CDividendLedger::CDividendLedger(const std::string& strLedgerFileIn) {
  strLedgerFile = strLedgerFileIn;
  fFileBacked = true;
  nTimeFirstKey = 0;
  Init();
}

void CDividendLedger::Init() {
  CChratosAddress address("cChratosDividendAddressxxxxvCMpCy");
  if (address.IsValid()) {
    CScript script = GetScriptForDestination(address.Get());
    if (HaveWatchOnly(script) && !AddWatchOnly(script)) {
      throw;
    }
  } else {
    throw;
  }
}

void CDividendLedger::MarkDirty() {
  {
    LOCK(cs_ledger);
    BOOST_FOREACH(PAIRTYPE(const uint256, CDividendTx)& item, mapLedger) {
      item.second.MarkDirty();
    }
  }
}

bool CDividendLedger::AddToLedgerIfDividend(const CTransaction &tx,
                                            const CBlock *pblock,
                                            bool fUpdate) {
  {
    LOCK(cs_ledger);

    bool fExisted = mapLedger.count(tx.GetHash()) != 0;

    if (fExisted && !fUpdate) { return false; }
    if (fExisted || IsDividend(tx)) {
      CDividendTx dtx(this, tx);

      CDividendLedgerDB ldb(strLedgerFile, "r+", false);

      return AddToLedger(dtx, false, &ldb);
    }
  }
}

bool CDividendLedger::IsDividend(const CTransaction &tx) {
  BOOST_FOREACH(const CTxOut &txout, tx.vout) {
    if (IsDividend(txout)) { return true; }
  }
  return false;
}

bool CDividendLedger::IsDividend(const CTxOut &txout) {
  return ::IsMine(*this, txout.scriptPubKey);
}

bool CDividendLedger::AddToLedger(const CDividendTx &dtxIn,
    bool fFromLoadLedger, 
    CDividendLedgerDB *pdividenddb) {

}

void CDividendLedger::SyncTransaction(const CTransaction& tx,
                                      const CBlockIndex *pindex,
                                      const CBlock* pblock,
                                      const bool fConnect) {
}

int CDividendLedger::ScanForDividendTransactions(CBlockIndex* pindexStart,
                                                 bool fUpdate) {
}

CAmount CDividendLedger::GetBalance() const {
  CAmount nTotal = 0;
  {
    LOCK2(cs_main, cs_ledger);
    for (auto &kv : mapLedger) {
      auto pcoin = &(kv.second);
      if (pcoin->IsTrusted()) {
        nTotal += pcoin->GetAvailableCredit();
      }
    }
  }
  return nTotal;
}

CAmount CDividendLedger::GetUnconfirmedBalance() const {
  CAmount nTotal = 0;
  return nTotal;
}

CAmount CDividendLedger::GetImmatureBalance() const {
  CAmount nTotal = 0;
  return nTotal;
}

bool CDividendLedger::InitLoadLedger() {
  std::string ledgerFile = GetArg("-dividend", DEFAULT_DIVIDEND_LEDGER_DAT);

  std::vector<CDividendTx> vDTx;

  uiInterface.InitMessage(_("Loading Dividend Ledger"));

  int64_t nStart = GetTimeMillis();
  
  bool fFirstRun = true;
  
  CDividendLedger *ledgerInstance = new CDividendLedger(ledgerFile);

  DBErrors nLoadLedgerRet = ledgerInstance->LoadLedger(fFirstRun);

  if (nLoadLedgerRet != DB_LOAD_OK) {
    if (nLoadLedgerRet == DB_CORRUPT) {
      return InitError(strprintf(_("Error loading %s: Dividend Ledger corrupted"), ledgerFile));
    } else if (nLoadLedgerRet == DB_NONCRITICAL_ERROR) {
      InitWarning(strprintf(_("Error reading %s!"), ledgerFile));
    } else if (nLoadLedgerRet == DB_TOO_NEW) {
      return InitError(strprintf(_("Error loading %s: Ledger requires newer version of %s"),
            ledgerFile, _(PACKAGE_NAME)));
    } else if (nLoadLedgerRet == DB_NEED_REWRITE) {
      return InitError(strprintf(_("Ledger needed to be rewritten: restart %s to complete"),
            _(PACKAGE_NAME)));
    } else {
      return InitError(strprintf(_("Error loading %s"), ledgerFile));
    }
  }

  if (fFirstRun) {
    //ledgerInstance->SetBestChain(chainActive.GetLocator());
  }

  pdividendLedgerMain = ledgerInstance;
}

std::map<uint256, CDividendTx> &CDividendLedger::GetMapLedger() const {
}

DBErrors CDividendLedger::LoadLedger(bool &fFirstRunRet) {
  if (!fFileBacked) {
    return DB_LOAD_OK;
  }

  fFirstRunRet = false;

  DBErrors nLoadLedgerRet = CDividendLedgerDB(strLedgerFile, "cr+").LoadLedger(this);
  if (nLoadLedgerRet == DB_NEED_REWRITE) {
    if (CDB::Rewrite(strLedgerFile, "\x04pool")) {
      LOCK(cs_ledger);
    }
  }

  if (nLoadLedgerRet != DB_LOAD_OK) {
    return nLoadLedgerRet;
  }

  uiInterface.LoadLedger(this);

  return DB_LOAD_OK;
}
