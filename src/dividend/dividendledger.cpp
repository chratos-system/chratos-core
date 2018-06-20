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
#include "checkpoints.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>

const char *DEFAULT_DIVIDEND_LEDGER_DAT = "dividend_ledger.dat";

CDividendLedger *pledgerMain = nullptr;

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

void CDividendLedger::Init() {}

void CDividendLedger::SetBestChain(const CBlockLocator &loc) {
  CDividendLedgerDB ledgerdb(strLedgerFile);
  ledgerdb.WriteBestBlock(loc);
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

      if (pblock) {
        dtx.SetMerkleBranch(*pblock);
      }

      CDividendLedgerDB ldb(strLedgerFile, "r+", false);

      return AddToLedger(dtx, false, &ldb);
    }
  }

  return false;
}

bool CDividendLedger::IsDividend(const CTransaction &tx) {
  BOOST_FOREACH(const CTxOut &txout, tx.vout) {
    if (IsDividend(txout)) { return true; }
  }
  return false;
}

bool CDividendLedger::IsDividend(const CTxOut &txout) const {
  return txout.IsDividendContribution();
}

bool CDividendLedger::AddToLedger(const CDividendTx &dtxIn,
                                  bool fFromLoadLedger, 
                                  CDividendLedgerDB *pdividenddb) {
  uint256 hash = dtxIn.GetHash();

  if (dtxIn.hashBlock == uint256()) { return false; }

  if (fFromLoadLedger) {
    mapLedger[hash] = dtxIn;
    CDividendTx &dtx = mapLedger[hash];
    dtxOrdered.insert(std::make_pair(dtx.GetBlockTime(), &dtx));
  } else {
    LOCK(cs_ledger);
    auto ret = mapLedger.insert(std::make_pair(hash, dtxIn));
    CDividendTx &dtx = (*ret.first).second;
    bool fInsertedNew = ret.second;

    if (fInsertedNew) {
      int64_t blocktime = mapBlockIndex[dtxIn.hashBlock]->GetBlockTime();
      dtx.SetBlockTime(blocktime);
      dtxOrdered.insert(std::make_pair(dtx.GetBlockTime(), &dtx));
    }

    bool fUpdated = false;

    if (!fInsertedNew) {
    }

    // Write to disk
    if (fInsertedNew || fUpdated) {
      if (!pdividenddb->WriteTx(dtx)) {
        return false;
      }
    }
  }

  return true;
}

void CDividendLedger::SyncTransaction(const CTransaction& tx,
                                      const CBlockIndex *pindex,
                                      const CBlock* pblock,
                                      const bool fConnect) {
  LOCK2(cs_main, cs_ledger);

  bool isDividend = true;

  if (!AddToLedgerIfDividend(tx, pblock, true)) {
    isDividend = false;
  }
}

int CDividendLedger::ScanForDividendTransactions(CBlockIndex* pindexStart,
                                                 bool fUpdate) {
  int ret = 0;
  int64_t nNow = GetTime();

  const CChainParams &chainParams = Params();

  CBlockIndex *pindex = pindexStart;
  {
    LOCK2(cs_main, cs_ledger);

    ShowProgress(_("Rescanning..."), 0);
    double dProgressStart = Checkpoints::GuessVerificationProgress(chainParams.Checkpoints(), pindex, false);
    double dProgressTip = Checkpoints::GuessVerificationProgress(chainParams.Checkpoints(), chainActive.Tip(), false);

    while (pindex) {
      if (pindex->nHeight % 100 == 0 && dProgressTip - dProgressStart > 0.0) {
        ShowProgress(_("Rescanning..."),
            std::max(1, std::min(99, (int)((Checkpoints::GuessVerificationProgress(chainParams.Checkpoints(), pindex, false) - dProgressStart) / (dProgressTip - dProgressStart) * 100))));
      }

      CBlock block;
      ReadBlockFromDisk(block, pindex, Params().GetConsensus());
      BOOST_FOREACH(CTransaction &tx, block.vtx) {
        if (AddToLedgerIfDividend(tx, &block, fUpdate)) {
          ret++;
        }
      }

      pindex = chainActive.Next(pindex);
      if (GetTime() >= nNow + 60) {
        nNow = GetTime();
        LogPrintf("Still ledger rescanning. At block %d. Progress=%f\n", pindex->nHeight, Checkpoints::GuessVerificationProgress(chainParams.Checkpoints(), pindex));
      }
    }
    ShowProgress(_("Rescanning..."), 100); // hide progress dialog in GUI
  }

  return ret;
}

CAmount CDividendLedger::GetBalance() const {
  CAmount nTotal = 0;
  {
    LOCK2(cs_main, cs_ledger);
    for (auto &kv : mapLedger) {
      auto pcoin = &(kv.second);
      nTotal += pcoin->GetDividendCredit();
    }
  }
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
    ledgerInstance->SetBestChain(chainActive.GetLocator());
  }

  RegisterValidationInterface(ledgerInstance);

  CBlockIndex *pindexRescan = chainActive.Tip();
  if (GetBoolArg("-rescan", false)) {
    pindexRescan = chainActive.Genesis();
  } else {
    CDividendLedgerDB ledgerdb(ledgerFile);
    CBlockLocator locator;
    if (ledgerdb.ReadBestBlock(locator)) {
      pindexRescan = FindForkInGlobalIndex(chainActive, locator);
    } else {
      pindexRescan = chainActive.Genesis();
    }
  }
  
  if (chainActive.Tip() && chainActive.Tip() != pindexRescan) {
    //We can't rescan beyond non-pruned blocks, stop and throw an error
    //this might happen if a user uses a old wallet within a pruned node
    // or if he ran -disablewallet for a longer time, then decided to re-enable
    if (fPruneMode) {
      CBlockIndex *block = chainActive.Tip();
      while (block && block->pprev && (block->pprev->nStatus & BLOCK_HAVE_DATA) && block->pprev->nTx > 0 && pindexRescan != block)
        block = block->pprev;
      if (pindexRescan != block) {
        return InitError(_("Prune: last ledger synchronisation goes beyond pruned data. You need to -reindex (download the whole blockchain again in case of pruned node)"));
      }
    }

    uiInterface.InitMessage(_("Rescanning..."));
    LogPrintf("Rescanning ledger last %i blocks (from block %i)...\n",
        chainActive.Height() - pindexRescan->nHeight, pindexRescan->nHeight);

    nStart = GetTimeMillis();
    ledgerInstance->ScanForDividendTransactions(pindexRescan, true);
    LogPrintf(" rescan      %15dms\n", GetTimeMillis() - nStart);
    ledgerInstance->SetBestChain(chainActive.GetLocator());
  }
 
  pledgerMain = ledgerInstance;

  return true;
}

std::map<uint256, CDividendTx> CDividendLedger::GetMapLedger() const {
  return mapLedger;
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

CAmount CDividendLedger::GetDividendCredit(const CTransaction& tx) const {
                         
  CAmount nCredit = 0;
  BOOST_FOREACH(const CTxOut& txout, tx.vout) {
    nCredit += GetDividendCredit(txout);
    if (!MoneyRange(nCredit)) {
      throw std::runtime_error("CDividendLedger::GetDividendCredit(): value out of range");
    }
  }
  return nCredit;
}

CAmount CDividendLedger::GetDividendCredit(const CTxOut& txout) const {
  if (!MoneyRange(txout.nValue)) {
    throw std::runtime_error("CWallet::GetDividendCredit(): value out of range");
  } 
  return (IsDividend(txout) ? txout.nValue : 0);
}

CDividendLedger::TxItems CDividendLedger::GetOrdered() const {
  return dtxOrdered;
}
