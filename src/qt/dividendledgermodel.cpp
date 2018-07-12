// Copyright (c) 2018 The Chratos Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "dividendledgermodel.h"
#include "dividend/dividendledger.h"
#include "dividend/dividendtx.h"
#include "main.h"
#include <boost/bind.hpp>
#include "dividendtablemodel.h"
#include "walletmodel.h"
#include "wallet/wallet.h"
#include "optionsmodel.h"
#include "chratosgui.h"
#include "guiutil.h"

DividendLedgerModel::DividendLedgerModel(
  const PlatformStyle *platformStyle,
  CDividendLedger *ledger, 
  WalletModel *walletModel,
  OptionsModel *optionsModel,
  QObject *parent
) : QObject(parent), ledger(ledger), optionsModel(optionsModel),
  walletModel(walletModel), fForceCheckDividendChanged(true),
  dividendTableModel(nullptr) {
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
  DividendLedgerModel *model,
  CDividendLedger *ledger,
  const uint256 &hash,
  ChangeType status
) {
  QMetaObject::invokeMethod(
    model, "updateDividend", Qt::QueuedConnection,
    Q_ARG(uint256, hash), Q_ARG(ChangeType, status)
  );
}

void DividendLedgerModel::subscribeToCoreSignals() {
  ledger->NotifyDividendChanged.connect(
    boost::bind(NotifyDividendChanged, this, _1, _2, _3)
  );
}

void DividendLedgerModel::unsubscribeFromCoreSignals() {
  ledger->NotifyDividendChanged.disconnect(boost::bind(NotifyDividendChanged, this, _1, _2, _3));
}

void DividendLedgerModel::updateDividend(
  const uint256 &hash, const ChangeType &status
) {
  fForceCheckDividendChanged = true;
  auto mi = ledger->GetMapLedger().find(hash);
  
  if (status == ChangeType::CT_NEW && mi != ledger->GetMapLedger().end()) {
    auto tx = (*mi).second;
    QString date = GUIUtil::dateTimeStr(tx.GetBlockTime());

    auto unit = optionsModel->getDisplayUnit();
    auto amount = walletModel->getWallet()->GetCreditFromDividend(tx);
    Q_EMIT incomingDividend(date, unit, amount);
  }

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

CAmount DividendLedgerModel::getAmountReceived(const CDividendTx &tx) const {
  if (this->walletModel && this->walletModel->getWallet()) {
    return this->walletModel->getWallet()->GetCreditFromDividend(tx);
  } else {
    return 0;
  }
}

void DividendLedgerModel::setChratosGUI(ChratosGUI *gui) {
  if (gui) {
    connect(this,
      SIGNAL(incomingDividend(const QString &, int, const CAmount &)),
      gui,
      SLOT(dividendReceived(const QString &, int, const CAmount &))
    );
  }
}
