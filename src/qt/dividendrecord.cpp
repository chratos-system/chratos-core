// Copyright (c) 2018 The Chratos Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "dividendrecord.h"

DividendRecord::DividendRecord(
  uint256 hash,
  qint64 time,
  const CAmount &amount,
  const CAmount &supply,
  double modifier,
  int64_t blockHeight
) : hash(hash), amount(amount), supply(supply),
  modifier(modifier), time(time), blockHeight(blockHeight) {
}

CAmount DividendRecord::getAmount() const {
  return amount;
}

CAmount DividendRecord::getSupply() const {
  return supply;
}

double DividendRecord::getModifier() const {
  return modifier;
}

uint256 DividendRecord::getHash() const {
  return hash;
}

qint64 DividendRecord::getTime() const {
  return time;
}

int64_t DividendRecord::getBlockHeight() const {
  return blockHeight;
}
