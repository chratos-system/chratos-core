// Copyright (c) 2018 The Chratos Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#ifndef CHRATOS_DIVIDEND_DIVIDEND_H
#define CHRATOS_DIVIDEND_DIVIDEND_H

#include "primitives/transaction.h"
#include "dividend/dividenddb.h"
#include "validationinterface.h"
#include "primitives/block.h"
#include "keystore.h"
#include "chain.h"
#include "dividend/rpcdividend.h"
#include "script/ismine.h"

extern CDividendLedger *pledgerMain;


class CDividendLedger : public CBasicKeyStore, public CValidationInterface {
  public:
    typedef std::multimap<int64_t, CDividendTx *> TxItems;
 
    CDividendLedger();

    CDividendLedger(const std::string& strLedgerFileIn);

    void Init();

    void MarkDirty();

    void SetBestChain(const CBlockLocator &loc);

    bool AddToLedgerIfDividend(const CTransaction &tx, const CBlock *pblock,
                               bool fUpdate);

    bool AddToLedger(const CDividendTx &dtxIn, bool fFromLoadLedger, 
                     CDividendLedgerDB *pdividenddb);

    void SyncTransaction(const CTransaction& tx, const CBlockIndex *pindex, 
                         const CBlock* pblock, const bool fConnect = true);

    int ScanForDividendTransactions(CBlockIndex* pindexStart,
                                    bool fUpdate = false);

    CAmount GetBalance() const;

    boost::signals2::signal<void (const std::string &title, int nProgress)> ShowProgress;

    bool LoadMinVersion(int nVersion) {
      AssertLockHeld(cs_ledger);
      nLedgerVersion = nVersion;
      nLedgerMaxVersion = std::max(nLedgerMaxVersion, nVersion);
      return true;
    }

    /* Initializes the ledger, returns a new CDividendLedger instance or a 
     * null pointer in case of an error.
     */
    static bool InitLoadLedger();

    DBErrors LoadLedger(bool &fFirstRunRet);

    std::map<uint256, CDividendTx> GetMapLedger() const;

    mutable CCriticalSection cs_ledger;

    CAmount GetCredit(const CTransaction& tx, const isminefilter& filter) const;

    CAmount GetCredit(const CTxOut& txout, const isminefilter& filter) const;

    TxItems GetOrdered() const;

  private:

    int64_t nTimeFirstKey;

    std::map<uint256, CDividendTx> mapLedger;

    bool fFileBacked;

    std::string strLedgerFile;

    int nLedgerVersion;

    int nLedgerMaxVersion;

    TxItems dtxOrdered;

    bool IsDividend(const CTransaction &ctx);

    bool IsDividend(const CTxOut &txout) const;
};

#endif
