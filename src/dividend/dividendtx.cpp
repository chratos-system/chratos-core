// Copyright (c) 2018 The Chratos Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "dividend/dividendtx.h"
#include "dividend/dividend.h"
#include "primitives/transaction.h"

CDividendTx::CDividendTx() {
  Init(nullptr);
}

CDividendTx::CDividendTx(const CDividendLedger* pledgerIn) {
  Init(pledgerIn);
}

CDividendTx::CDividendTx(const CDividendLedger* pledgerIn, const CTransaction& txIn) : CTransaction(txIn) {
  Init(pledgerIn);
}

void CDividendTx::Init(const CDividendLedger* pledgerIn) {
  ledger = pledgerIn;
}

void CDividendTx::MarkDirty() {
}

bool CDividendTx::IsTrusted() const {
  return true;
}

CAmount CDividendTx::GetAvailableCredit() const {
  return 0;
}
