// Copyright (c) 2018 The Chratos Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "dividend/dividend.h"
#include "dividend/dividendledger.h"
#include "dividend/dividendtx.h"

#include "main.h"
#include "chainparams.h"
#include "pos.h"

#include <algorithm>

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

bool CDividend::ExceedsThresholdSolo(const CDividendTx &tx) {
  if (tx.isImmature()) { return false; }
  return (tx.GetPayoutModifier() >= CDividend::DIVIDEND_THRESHOLD);
}

bool CDividend::ExceedsThresholdWithPriors(const CDividendTx &tx) {
  if (!tx.isImmature()) { return false; }

  auto priors = pledgerMain->GetPayoutsBefore(tx);
  std::reverse(priors.begin(), priors.end());
  std::vector<CDividendTx> unpaids;

  for (auto &dtx : priors) {
    if (dtx.GetHeight() != tx.GetHeight()) {
      if (dtx.isImmature()) { return false; }
      if (ExceedsThresholdSolo(dtx)) { break; }
      if (ExceedsThresholdWithPriors(dtx)) { break; }
    }
    unpaids.push_back(tx);
  }

  CAmount total = tx.GetDividendCredit();
  const CAmount supply = tx.GetCoinSupply();
  for (auto &unpaid : unpaids) {
    total += unpaid.GetDividendCredit();
  }

  const auto modifier = GetModifier(total, supply);
  return modifier >= CDividend::DIVIDEND_THRESHOLD;
}

bool CDividend::ExceedsThresholdWithSubsequents(
  const CDividendTx &tx,
  const int blockHeight
) {

  auto laters = pledgerMain->GetPayoutsAfter(tx, blockHeight);

  std::vector<CDividendTx> unpaids;

  for (auto &dtx : laters) {
    if (ExceedsThresholdSolo(dtx)) {
      return true;
    } else if (ExceedsThresholdWithPriors(dtx)) { 
      return true;
    }
  }

  return false;
}

bool CDividend::DoesExceedThreshold(const CDividendTx &tx) {
  if (tx.isImmature()) { return false; }
  return ExceedsThresholdSolo(tx) || ExceedsThresholdWithPriors(tx);
}

bool CDividend::ExceedsThresholdAt(
  const CDividendTx &tx,
  const int blockHeight
) {
  if (tx.isImmature()) { return false; }

  if (ExceedsThresholdSolo(tx)) {
    return true;
  } else if (ExceedsThresholdWithPriors(tx)) {
    return true;
  } else if (ExceedsThresholdWithSubsequents(tx, blockHeight)) {
    return true;
  }

  return false;
}

CAmount CDividend::GetMoneySupplyAtHeight(int height) {
  const auto params = Params().GetConsensus();
  const auto lastPOW = params.nLastPOWBlock;

  CAmount total = 0;
  for (int i = 0; i < height; i++) {
    if (i <= lastPOW) {
      total += GetBlockSubsidy(i, params);
    } else {
      total += GetProofOfStakeReward(i, 0, 0);
    }
  }

  return total;
}

double CDividend::GetModifier(const CAmount &amount, const CAmount &supply) {
  return (double)amount / (double)(supply - amount);
}

CAmount CDividend::GetDividendFundAt(int blockHeight) {

  CAmount total = 0;
  std::vector<CDividendTx> fundTxs;
  auto dividends = pledgerMain->GetOrdered();
  
  for (auto &dividend : dividends) {
    if (dividend.GetMatureHeight() <= blockHeight) {
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

  for (auto &dividend : dividends) {

    if (dividend.GetMatureHeight() >= blockHeight && 
        DoesExceedThreshold(dividend) &&
        dividend.isMature()) {

      auto chain = GetExceedChainFor(dividend);

      CAmount total = 0;
      for (auto &it : chain) {
        total += it.GetDividendCredit();
      }

      if (total == 0) { continue; }

      arith_uint256 fund = arith_uint256(total);

      arith_uint256 supply = arith_uint256(
          dividend.GetCoinSupply() - dividend.GetDividendCredit()
      );

      auto mint = bigAmount * fund / supply;

      bigAmount = bigAmount + mint;
    }
  }

  auto total = bigAmount.GetLow64();

  return total;
}

CAmount CDividend::GetDividendPayoutUntil(
  CAmount amount, int blockHeight, int untilHeight
) {

  arith_uint256 bigAmount = arith_uint256(amount);

  auto dividends = pledgerMain->GetOrdered();

  for (auto &dividend : dividends) {

    if (dividend.GetMatureHeight() >= blockHeight &&
        dividend.GetMatureHeight() < untilHeight &&
        dividend.isMature() &&
        DoesExceedThreshold(dividend)) {

      auto chain = GetExceedChainFor(dividend);
      CAmount total = 0;
      for (auto &it : chain) {
        total += it.GetDividendCredit();
      }

      if (total == 0) { continue; }

      arith_uint256 fund = arith_uint256(total);

      arith_uint256 supply = arith_uint256(
          dividend.GetCoinSupply() - dividend.GetDividendCredit()
      );

      auto mint = bigAmount * fund / supply;

      bigAmount = bigAmount + mint;
    }
  }

  auto total = bigAmount.GetLow64();

  return total;

}

CAmount CDividend::GetTotalPaid() {
  auto dividends = pledgerMain->GetOrdered();
  CAmount total = 0;
  int height = chainActive.Tip()->nHeight;

  for (auto &it : dividends) {
    if (ExceedsThresholdAt(it, height)) {
      total += it.GetDividendCredit();
    }
  }
  return total;
}

int CDividend::GetPaidBlockBefore(const CDividendTx &tx) {
  const int starting = tx.GetHeight();

  auto priors = pledgerMain->GetPayoutsBefore(tx);

  int ending = -1;

  for (auto &dtx : priors) {
    if (dtx.GetHeight() == tx.GetHeight()) { continue; }

    if (DoesExceedThreshold(dtx)) {
      ending = dtx.GetHeight();
    }
  }

  return ending;
}

std::vector<CDividendTx> CDividend::GetExceedChainFor(const CDividendTx &tx) {
  std::vector<CDividendTx> chain;

  if (DoesExceedThreshold(tx)) {
    auto lastPaidHeight = GetPaidBlockBefore(tx);
    auto priors = pledgerMain->GetPayoutsBefore(tx, lastPaidHeight);

    const CAmount supply = tx.GetCoinSupply();
    CAmount total = 0;
    for (auto &dtx : priors) {
      total += dtx.GetDividendCredit();
    }

    if (GetModifier(total, supply) > CDividend::DIVIDEND_THRESHOLD) {
      chain.insert(std::end(chain), std::begin(priors), std::end(priors));
    }

    chain.push_back(tx);
  }

  return chain;
}
