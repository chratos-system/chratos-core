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

bool CDividendTx::isMature() const {
  const auto height = GetHeight();
  return isMatureAt(height);
}

bool CDividendTx::isImmature() const {
  return !isMature();
}

bool CDividendTx::isMatureAt(int64_t height) const {
  const auto tip = chainActive.Tip()->nHeight;
  return tip >= height + CDividend::DIVIDEND_MIN_AGE;
}

CAmount CDividendTx::GetDividendCredit() const {

  CAmount nCredit = 0;

  for (auto &txout : vout) {
    nCredit += ledger->GetDividendCredit(txout);
    if (!MoneyRange(nCredit)) {
      throw std::runtime_error(
        "CDividendTx::GetAvailableCredit() : value out of range"
      );
    }
  }

  return nCredit;
}

CAmount CDividendTx::GetCoinSupply() const {
  const auto height = GetMatureHeight();
  CAmount supply = CDividend::GetMoneySupplyAtHeight(height);
  return supply;
}

int64_t CDividendTx::GetHeight() const {
  auto index = GetBlock();
  if (index) {
    return index->nHeight;
  } else {
    return -1;
  }
}

int64_t CDividendTx::GetMatureHeight() const {
  auto height = GetHeight();
  if (height > 0) { height += CDividend::DIVIDEND_MIN_AGE; }
  return height;
}

int64_t CDividendTx::GetBlockTime() const {
  auto index = GetBlock();
  if (index) { 
    return index->GetBlockTime();
  } else {
    return -1;
  }
}

void CDividendTx::SetBlockTime(int64_t bt) {
  blockTime = bt;
}

double CDividendTx::GetPayoutModifier() const {
  return CDividend::GetModifier(GetDividendCredit(), GetCoinSupply());
}

CBlockIndex *CDividendTx::GetBlock() const {
  BlockMap::iterator mi = mapBlockIndex.find(hashBlock);
  if (mi == mapBlockIndex.end()) {
    return nullptr;
  } else {
    return (*mi).second;
  }
}
