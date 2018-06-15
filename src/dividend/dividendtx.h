// Copyright (c) 2018 The Chratos Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#ifndef CHRATOS_DIVIDEND_DIVIDEND_TX_H
#define CHRATOS_DIVIDEND_DIVIDEND_TX_H

#include "wallet/wallet.h"

class CDividendLedger;
class CBlock;

class CDividendTx : public CMerkleTx {
  public:

    CDividendTx();

    CDividendTx(const CDividendLedger *pledgerIn,
                const CMerkleTx &txIn);

    CDividendTx(const CDividendLedger *pledgerIn,
                const CTransaction &txIn);

    void MarkDirty();

    bool IsTrusted() const;

    CAmount GetAvailableCredit() const;

    void Init(const CDividendLedger *pledgerIn);

    int64_t GetBlockTime() const;

    void SetBlockTime(int64_t);

  private:

    const CDividendLedger *ledger;

    int64_t blockTime;

    uint256 blockHash;
};

#endif

