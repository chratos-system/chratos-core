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

    bool isMature() const;

    bool isImmature() const;

    bool isMatureAt(int64_t) const;

    CAmount GetDividendCredit() const;

    void Init(const CDividendLedger *pledgerIn);

    int64_t GetBlockTime() const;

    int64_t GetHeight() const;

    int64_t GetMatureHeight() const;

    void SetBlockTime(int64_t);

    CAmount GetCoinSupply() const;

    double GetPayoutModifier() const;

  private:

    const CDividendLedger *ledger;

    int64_t blockTime;

    uint256 blockHash;

    CBlockIndex *GetBlock() const;
};

#endif

