// Copyright (c) 2018 The Chratos Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#ifndef CHRATOS_DIVIDEND_DIVIDEND_TX_H
#define CHRATOS_DIVIDEND_DIVIDEND_TX_H

#include "primitives/transaction.h"

class CDividendLedger;

class CDividendTx : public CTransaction {
  public:

    CDividendTx();
    CDividendTx(const CDividendLedger *pledgerIn);
    CDividendTx(const CDividendLedger *pledgerIn, const CTransaction &txIn);

    void MarkDirty();
    bool IsTrusted() const;
    CAmount GetAvailableCredit() const;

    void Init(const CDividendLedger *pledgerIn);

  private:

    const CDividendLedger *ledger;
};

#endif

