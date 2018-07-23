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

CAmount CDividend::GetDividendFundAt(int blockHeight) {
  auto dividends = pledgerMain->GetOrdered();

  std::vector<CDividendTx> fundTxs;
  
  for (auto &it : dividends) {
    auto dividend = *(it.second);
    if (dividend.GetHeight() <= blockHeight) {
      fundTxs.push_back(dividend);
    }
  }

  CAmount total;
  for (auto &it : fundTxs) {
    total += it.GetDividendCredit();
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
