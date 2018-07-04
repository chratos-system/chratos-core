// Copyright (c) 2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "dividendtablemodel.h"

#include "util.h"
#include "dividend/dividendledger.h"
#include "dividendview.h"
#include "platformstyle.h"
#include "dividendledgermodel.h"
#include "guiutil.h"
#include "optionsmodel.h"

#include <QDateTime>
#include <QColor>
#include <QDebug>
#include <QIcon>
#include <QList>

static int column_alignments[] = {
  Qt::AlignLeft|Qt::AlignVCenter, /* date */
  Qt::AlignLeft|Qt::AlignVCenter, /* Transaction ID */
  Qt::AlignRight|Qt::AlignVCenter, /* Block Height */
  Qt::AlignRight|Qt::AlignVCenter, /* Amount */
  Qt::AlignRight|Qt::AlignVCenter, /* Money Supply */
  Qt::AlignRight|Qt::AlignVCenter /* % */
};

struct DivLessThan {
  bool operator() (
    const DividendRecord &a, const DividendRecord &b
  ) const {
    return a.getHash() < b.getHash();
  }

  bool operator()(const DividendRecord &a, const uint256 &b) const {
    return a.getHash() < b;
  }

  bool operator()(const uint256 &a, const DividendRecord &b) const {
    return a < b.getHash();
  }
};

DividendTableModel::DividendTableModel(
  const PlatformStyle *platformStyle,
  DividendLedgerModel *ledgerModel,
  CDividendLedger *ledger,
  DividendView *parent
) : QAbstractTableModel(parent), ledgerModel(ledgerModel),
  dividendView(parent), ledger(ledger),
  platformStyle(platformStyle) {

  columns << tr("Date")
    << tr("Transaction ID")
    << tr("Block Height")
    << ChratosUnits::getAmountColumnTitle(
      ledgerModel->getOptionsModel()->getDisplayUnit()
    )
    << ChratosUnits::getMoneySupplyColumnTitle(
      ledgerModel->getOptionsModel()->getDisplayUnit()
    )
    << tr("Received");

  setupTransactions();
  connect(ledgerModel->getOptionsModel(), SIGNAL(displayUnitChanged(int)),
    this, SLOT(updateDisplayUnit()));

  subscribeToCoreSignals();
}

void DividendTableModel::setupTransactions() {

  cachedLedger.clear();

  std::vector<CDividendTx> transactions;

  for (auto &it : ledgerModel->getTransactions()) {
    transactions.push_back(it.second);
  }

  std::sort(transactions.begin(), transactions.end(), 
      [](CDividendTx a, CDividendTx b) {
    return a.GetHeight() > b.GetHeight();
  });


  for (auto &it : transactions) {
    auto record = DividendRecord(
      it.GetHash(),
      it.GetBlockTime(),
      it.GetDividendCredit(),
      it.GetCoinSupply(),
      it.GetPayoutModifier(),
      it.GetHeight(),
      ledgerModel->getAmountReceived(it)
    );

    cachedLedger.append(record);
  }
}


DividendTableModel::~DividendTableModel() {
  unsubscribeFromCoreSignals();
}

int DividendTableModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return ledgerModel->getCount();
}

int DividendTableModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return columns.length();
}

QVariant DividendTableModel::data(const QModelIndex &index, int role) const {
  if(!index.isValid()) {
    return QVariant();
  }

  DividendRecord *rec = static_cast<DividendRecord *>(index.internalPointer());

  switch (role) {
    case Qt::DisplayRole:
      switch (index.column()) {
        case Date:
          return formatDivDate(rec);
        case Amount:
          return formatDivAmount(rec, ChratosUnits::separatorAlways);
        case BlockHeight:
          return formatDivHeight(rec);
        case TransactionID:
          return formatTxId(rec);
        case MoneySupply:
          return formatMoneySupply(rec, ChratosUnits::separatorAlways);
        case Received:
          return formatReceived(rec, ChratosUnits::separatorAlways);
      }
      break;
    case Qt::ToolTipRole:
      return "";
    case Qt::TextAlignmentRole:
      return column_alignments[index.column()];
  }


  return QVariant();
}

QVariant DividendTableModel::headerData(
  int section, Qt::Orientation orientation, int role
) const {
  if(orientation == Qt::Horizontal) {
    if(role == Qt::DisplayRole) {
      return columns[section];
    } else if (role == Qt::TextAlignmentRole) {
      return column_alignments[section];
    } else if (role == Qt::ToolTipRole) {
      switch(section) {
        case Date:
          return tr("Date and time that the transaction was received.");
        case Amount:
          return tr("Amount paid out to holders.");
      }
    }
  }
  return QVariant();
}

QModelIndex DividendTableModel::index(
  int row, int column, const QModelIndex &parent
) const {
  Q_UNUSED(parent);

  DividendRecord *data = (DividendRecord *)&cachedLedger[row];

  if (data) {
    return createIndex(row, column, data);
  }

  return QModelIndex();
}


QString DividendTableModel::formatDivDate(const DividendRecord *rec) const {
  if(rec->getTime()) {
    return GUIUtil::dateTimeStr(rec->getTime());
  }
  return QString();
}

QString DividendTableModel::formatMoneySupply(
    const DividendRecord *div, 
    ChratosUnits::SeparatorStyle separators
    ) const {
  auto amount = div->getSupply();
  return formatAmount(amount, separators);
}

QString DividendTableModel::formatReceived(
  const DividendRecord *div, 
  ChratosUnits::SeparatorStyle separators
) const {
  auto amount = div->getReceivedAmount();
  return formatAmount(amount, separators);
}

QString DividendTableModel::formatDivAmount(
  const DividendRecord *div,
  ChratosUnits::SeparatorStyle separators
) const {
  auto amount = div->getAmount();
  return formatAmount(amount, separators);
}

QString DividendTableModel::formatAmount(
  const CAmount &amount,
  ChratosUnits::SeparatorStyle separators
) const {
  QString str = ChratosUnits::format(
    ledgerModel->getOptionsModel()->getDisplayUnit(),
    amount,
    false,
    separators
  );

  return QString(str);
}

QString DividendTableModel::formatDivHeight(const DividendRecord *rec) const {
  auto i = rec->getBlockHeight();
  return QString::number(i);
}

QString DividendTableModel::formatTxId(const DividendRecord *rec) const {
  auto tx = rec->getHash().ToString();
  return QString::fromUtf8(tx.c_str());
}

QString DividendTableModel::formatPercentage(const DividendRecord *rec) const {
  auto i = (rec->getModifier()  * 100);
  return QString::number(i);
}

void DividendTableModel::updateDividend(
  const QString &hash, int status, bool showTransaction
) {

  setupTransactions();
  layoutChanged();
}

void DividendTableModel::updateConfirmations() {
  // There's nothing needed here initially
}

void DividendTableModel::updateDisplayUnit() {
  updateAmountColumnTitle();
  if (cachedLedger.size() > 0) {
    Q_EMIT dataChanged(
      index(0, Amount), index(cachedLedger.size() - 1, Amount)
    );
    Q_EMIT dataChanged(
      index(0, MoneySupply), index(cachedLedger.size() - 1, MoneySupply)
    );
    Q_EMIT dataChanged(
      index(0,  Received), index(cachedLedger.size() - 1, Received)
    );
  }
}

void DividendTableModel::updateAmountColumnTitle() {
  columns[Amount] = ChratosUnits::getAmountColumnTitle(
    ledgerModel->getOptionsModel()->getDisplayUnit()
  );
  columns[MoneySupply] = ChratosUnits::getMoneySupplyColumnTitle(
    ledgerModel->getOptionsModel()->getDisplayUnit()
  );
  columns[Received] = ChratosUnits::getReceivedColumnTitle(
    ledgerModel->getOptionsModel()->getDisplayUnit()
  );
  Q_EMIT headerDataChanged(Qt::Horizontal, Amount, Amount);
  Q_EMIT headerDataChanged(Qt::Horizontal, MoneySupply, MoneySupply);
}

struct DividendNotification {
  public:
    DividendNotification() {}
    DividendNotification(
      uint256 hash, ChangeType status, bool showTransaction
    ) : hash(hash), status(status), showTransaction(showTransaction) {}
    
    void invoke(QObject *ttm) {
      QString strHash = QString::fromStdString(hash.GetHex());
      QMetaObject::invokeMethod(ttm, "updateDividend", Qt::QueuedConnection,
          Q_ARG(QString, strHash),
          Q_ARG(int, status),
          Q_ARG(bool, showTransaction));
    }

  private:
    uint256 hash;
    ChangeType status;
    bool showTransaction;
};

static void NotifyDividendChanged(
  DividendTableModel *model, CDividendLedger *ledger, const uint256 &hash,
  ChangeType status
) {
  auto mi = ledger->GetMapLedger().find(hash);
  bool inLedger = mi != ledger->GetMapLedger().end();

  bool showTransaction = inLedger;
  
  DividendNotification notification(hash, status, showTransaction);

  notification.invoke(model);
}

void DividendTableModel::subscribeToCoreSignals() {
  ledger->NotifyDividendChanged.connect(
    boost::bind(NotifyDividendChanged, this, _1, _2, _3)
  );
}

void DividendTableModel::unsubscribeFromCoreSignals() {
  ledger->NotifyDividendChanged.disconnect(
    boost::bind(NotifyDividendChanged, this, _1, _2, _3)
  );
}
