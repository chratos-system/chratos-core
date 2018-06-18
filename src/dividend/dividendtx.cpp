// Copyright (c) 2018 The Chratos Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "dividend/dividendtx.h"
#include "dividend/dividend.h"
#include "primitives/transaction.h"
#include "primitives/block.h"
#include "main.h"

CDividendTx::CDividendTx() {
  Init(nullptr);
}

CDividendTx::CDividendTx(const CDividendLedger* pledgerIn,
                         const CMerkleTx& txIn) : CMerkleTx(txIn) {
  Init(pledgerIn);
}

CDividendTx::CDividendTx(const CDividendLedger* pledgerIn,
                         const CTransaction& txIn) : CMerkleTx(txIn) {
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

  CAmount nCredit = 0;

  for (auto &txout : vout) {
    nCredit += ledger->GetCredit(txout, ISMINE_WATCH_ONLY);
    if (!MoneyRange(nCredit)) {
      throw std::runtime_error("CDividendTx::GetAvailableCredit() : value out of range");
    }
  }

  return nCredit;
}

CAmount CDividendTx::GetCoinSupply() const {
  CAmount supply = GetBlock()->nMoneySupply * COIN;
  return supply;
}

int64_t CDividendTx::GetBlockTime() const {
  return GetBlock()->GetBlockTime();
}

void CDividendTx::SetBlockTime(int64_t bt) {
  blockTime = bt;
}

CBlockIndex *CDividendTx::GetBlock() const {
  return mapBlockIndex[hashBlock];
}
