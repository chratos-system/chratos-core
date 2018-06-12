// Copyright (c) 2018 The Chratos Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "dividend/dividend.h"
#include "dividend/dividenddb.h"

#include "ui_interface.h"
#include "main.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>

const char *DEFAULT_DIVIDEND_LEDGER_DAT = "dividendledger.dat";

CDividendLedger::CDividendLedger() {
  fFileBacked = false;
  nTimeFirstKey = 0;
}

CDividendLedger::CDividendLedger(const std::string& strLedgerFileIn) {
  strLedgerFile = strLedgerFileIn;
  fFileBacked = true;
  nTimeFirstKey = 0;
}

void CDividendLedger::MarkDirty() {
  {
    LOCK(cs_ledger);
    BOOST_FOREACH(PAIRTYPE(const uint256, CDividendTx)& item, mapLedger) {
      item.second.MarkDirty();
    }
  }
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
