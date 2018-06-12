// Copyright (c) 2018 The Chratos Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "dividend/dividend.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>

const char *DEFAULT_DIVIDEND_LEDGER_DAT = "dividendLedger.dat";

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
  LOCK(cs_ledger);
  BOOST_FOREACH(PAIRTYPE(const uint256, CDividendTx)& item, mapLedger) {
    item.second.MarkDirty();
  }
}

bool CDividendLedger::AddToLedger(const CDividendTx &dtxIn,
    bool fFromLoadLedger, 
    CDividendDB *pdividenddb) {

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
}

CAmount CDividendLedger::GetUnconfirmedBalance() const {
}

CAmount CDividendLedger::GetImmatureBalance() const {
}

bool CDividendLedger::InitLoadLedger() {
  std::string ledgerFile = GetArg("-dividend", DEFAULT_DIVIDEND_LEDGER_DAT);

  std::vector<CDividendTx> vDTx;

  uiInterface.InitMessage(_("Loading Dividend Ledger"));

  int64_t nStart = GetTimeMillis();
  bool fFirstRun = true;
  CDividendLedger *ledgerInstance = new CDividendLedger(walletFile);
  DBErrors nLoadWalletRet = ledgerInstance->LoadLedger(fFirstRun);
}

std::map<uint256, CDividendTx> &CDividendLedger::GetMapLedger() const {
}

DBErrors CDividendLedger::LoadWallet(bool &fFirstRunRet) {
    if (!fFileBacked) {
        return DB_LOAD_OK;
    }
    fFirstRunRet = false;
    DBErrors nLoadWalletRet = CDividendLedgerDB(strLedgerFile,"cr+").LoadLedger(this);
    if (nLoadWalletRet == DB_NEED_REWRITE) {
        if (CDB::Rewrite(strWalletFile, "\x04pool")) {
            LOCK(cs_ledger);
        }
    }

    if (nLoadWalletRet != DB_LOAD_OK) {
        return nLoadLedgerRet;
    }

    fFirstRunRet = !vchDefaultKey.IsValid();

    uiInterface.LoadLedger(this);

    return DB_LOAD_OK;
}
