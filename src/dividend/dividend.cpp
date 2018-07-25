// Copyright (c) 2018 The Chratos Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "dividend/dividend.h"
#include "dividend/dividendledger.h"
#include "dividend/dividendtx.h"

void CDividend::SetScriptForDividendContribution(CScript &script) {
  script.resize(2);
  script[0] = OP_RETURN;
  script[1] = OP_DIVIDEND;
}

CAmount CDividend::GetDividendPayout(CAmount amount, int blockHeight) {
  auto total = GetTotalWithDividend(amount, blockHeight);

  if (total > amount) {
    return total - amount;
  } else {
    return 0;
  }
}

bool CDividend::ExceedsThreshold(const CDividendTx &tx) {
  return (tx.GetPayoutModifier() >= CDividend::DIVIDEND_THRESHOLD);
}

bool CDividend::ExceedsThresholdWithPriors(const CDividendTx &tx) {
  auto priors = pledgerMain->GetPayoutsBefore(tx);
  std::vector<CDividendTx> unpaids;

  for (auto &dtx : priors) {
    if (ExceedsThreshold(dtx)) { break; }
    if (ExceedsThresholdWithPriors(tx)) { break; }
    unpaids.push_back(tx);
  }

  CAmount total = tx.GetDividendCredit();
  const CAmount supply = tx.GetCoinSupply();
  for (auto &unpaid : unpaids) {
    total += unpaid.GetDividendCredit();
  }

  const auto modifier = double(total) / double(supply);
  return modifier >= CDividend::DIVIDEND_THRESHOLD;
}

bool CDividend::ExceedsThresholdWithSubsequents(
  const CDividend &tx,
  const int blockHeight
) {

  auto laters = pledgerMain->GetPayoutsAfter(tx, blockHeight);
  std::vector<CDividendTx> unpaids;

  for (auto &dtx : laters) {
    if (ExceedsThreshold(dtx)) { break; }
    if (ExceedsThresholdWithPriors(tx)) { 
      return true;
    }
  }

  return false;
}

bool CDividend::ExceedsThresholdAt(const CDividendTx &tx, int blockHeight) {
  if (ExceedsThreshold(tx)) {
    return true;
  } else if (ExceedsThresholdWithPriors(tx)) {
    return true;
  } else if (ExceedsThresholdWithSubsequents(tx, blockHeight)) {
    return true;
  }

  return false;
}

CAmount CDividend::GetDividendFundAt(int blockHeight) {

  CAmount total = 0;
  std::vector<CDividendTx> fundTxs;
  auto dividends = pledgerMain->GetOrdered();
  
  for (auto &it : dividends) {
    auto dividend = *(it.second);
    if (dividend.GetHeight() <= blockHeight) {
      fundTxs.push_back(dividend);
    }
  }

  for (auto &it : fundTxs) {
    if (!CDividend::ExceedsThresholdAt(it, blockHeight)) {
      total += it.GetDividendCredit();
    }
  }

  return total;
}

CAmount CDividend::GetCurrentDividendFund() {
  return CDividend::GetDividendFundAt(chainActive.Height());
}

CAmount CDividend::GetTotalWithDividend(CAmount amount, int blockHeight) {

  arith_uint256 bigAmount = arith_uint256(amount);

  auto dividends = pledgerMain->GetOrdered();

  for (auto &it : dividends) {

    auto dividend = *(it.second);

    if (dividend.GetHeight() >= blockHeight) {
      arith_uint256 fund = arith_uint256(dividend.GetDividendCredit());

      arith_uint256 supply = arith_uint256(
          dividend.GetCoinSupply() - dividend.GetDividendCredit()
      );

      auto mint = bigAmount * fund / supply;

      bigAmount = bigAmount + mint;
    }
  }

  auto total = bigAmount.GetLow64();

  return amount;//total;
}

CAmount CDividend::GetDividendPayoutUntil(
  CAmount amount, int blockHeight, int untilHeight
) {

  arith_uint256 bigAmount = arith_uint256(amount);

  auto dividends = pledgerMain->GetOrdered();

  for (auto &it : dividends) {

    auto dividend = *(it.second);

    if (dividend.GetHeight() >= blockHeight &&
        dividend.GetHeight() < untilHeight) {
      arith_uint256 fund = arith_uint256(dividend.GetDividendCredit());

      arith_uint256 supply = arith_uint256(
          dividend.GetCoinSupply() - dividend.GetDividendCredit()
      );

      auto mint = bigAmount * fund / supply;

      bigAmount = bigAmount + mint;
    }
  }

  auto total = bigAmount.GetLow64();

  return amount;//total;

}
