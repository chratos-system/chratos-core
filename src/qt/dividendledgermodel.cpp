// Copyright (c) 2018 The Chratos Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "dividendledgermodel.h"
#include "dividend/dividendledger.h"
#include "main.h"

DividendLedgerModel::DividendLedgerModel(
  const PlatformStyle *platformStyle,
  CDividendLedger *ledger, 
  OptionsModel *optionsModel,
  QObject *parent
) : QObject(parent), ledger(ledger), optionsModel(optionsModel) {
}

CAmount DividendLedgerModel::getTotalDividendFund() const {
  return ledger->GetBalance();
}

OptionsModel *DividendLedgerModel::getOptionsModel() {
  return optionsModel;
}
