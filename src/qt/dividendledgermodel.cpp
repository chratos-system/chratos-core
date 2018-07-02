// Copyright (c) 2018 The Chratos Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "dividendledgermodel.h"
#include "dividend/dividendledger.h"
#include "dividend/dividendtx.h"
#include "main.h"
#include <boost/bind.hpp>
#include "dividendtablemodel.h"

DividendLedgerModel::DividendLedgerModel(
  const PlatformStyle *platformStyle,
  CDividendLedger *ledger, 
  OptionsModel *optionsModel,
  QObject *parent
) : QObject(parent), ledger(ledger), optionsModel(optionsModel), 
  fForceCheckDividendChanged(true), dividendTableModel(nullptr) {
}

CAmount DividendLedgerModel::getTotalDividendFund() const {
  return ledger->GetBalance();
}

OptionsModel *DividendLedgerModel::getOptionsModel() {
  return optionsModel;
}

int DividendLedgerModel::getCount() const {
  return ledger->GetOrdered().size();
}

std::map<uint256, CDividendTx> DividendLedgerModel::getTransactions() const {
  return ledger->GetMapLedger();
}

static void NotifyDividendChanged(
  DividendLedgerModel *model, CDividendLedger *ledger,
  const uint256 &hash, ChangeType status
) {
  Q_UNUSED(ledger);
  Q_UNUSED(hash);
  Q_UNUSED(status);
  QMetaObject::invokeMethod(model, "updateDividend", Qt::QueuedConnection);
}

void DividendLedgerModel::subscribeToCoreSignals() {
  ledger->NotifyDividendChanged.connect(
    boost::bind(NotifyDividendChanged, this, _1, _2, _3)
  );
}

void DividendLedgerModel::unsubscribeFromCoreSignals() {
  ledger->NotifyDividendChanged.disconnect(boost::bind(NotifyDividendChanged, this, _1, _2, _3));
}

void DividendLedgerModel::updateDividend() {
  fForceCheckDividendChanged = true;
}

void DividendLedgerModel::pollDividendChange() {
  TRY_LOCK(cs_main, lockMain);
  if (!lockMain) { return; }

  TRY_LOCK(ledger->cs_ledger, lockLedger);
  if (!lockLedger) { return; }

  if (fForceCheckDividendChanged || chainActive.Height() != cachedNumBlocks) {
    fForceCheckDividendChanged = false;

    if (dividendTableModel) {
      dividendTableModel->updateConfirmations();
    }
  }
}

CDividendLedger *DividendLedgerModel::getLedger() const {
  return ledger;
}

void DividendLedgerModel::setDividendTableModel(
  DividendTableModel *dividendTableModel
) {
  this->dividendTableModel = dividendTableModel;
}
