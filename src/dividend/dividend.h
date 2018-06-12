// Copyright (c) 2018 The Chratos Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#ifndef CHRATOS_DIVIDEND_DIVIDEND_H
#define CHRATOS_DIVIDEND_DIVIDEND_H

#include "primitives/transaction.h"
#include "dividend/dividenddb.h"
#include "primitives/block.h"
#include "chain.h"

class CDividendTx : public CTransaction {
  public:

    void MarkDirty();
  private:

};

class CDividendLedger {
  public:

    CDividendLedger();
    CDividendLedger(const std::string& strLedgerFileIn);

    void MarkDirty();

    bool AddToLedger(const CDividendTx &dtxIn, bool fFromLoadLedger, 
                     CDividendDB *pdividenddb);

    void SyncTransaction(const CTransaction& tx, const CBlockIndex *pindex, 
                         const CBlock* pblock, const bool fConnect = true);

    int ScanForDividendTransactions(CBlockIndex* pindexStart, bool fUpdate = false);

    CAmount GetBalance() const;

    CAmount GetUnconfirmedBalance() const;

    CAmount GetImmatureBalance() const;

    /* Initializes the ledger, returns a new CDividendLedger instance or a 
     * null pointer in case of an error.
     */
    static bool InitLoadLedger();

    std::map<uint256, CDividendTx> &GetMapLedger() const;

  private:

    int64_t nTimeFirstKey;

    std::map<uint256, CDividendTx> mapLedger;

    bool fFileBacked;

    std::string strLedgerFile;

    mutable CCriticalSection cs_ledger;
};

#endif
