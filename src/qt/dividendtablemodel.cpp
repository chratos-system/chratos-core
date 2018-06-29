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

DividendTableModel::DividendTableModel(
  const PlatformStyle *platformStyle,
  DividendLedgerModel *ledger,
  DividendView *parent
) : QAbstractTableModel(parent), ledgerModel(ledger), dividendView(parent), 
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
    << tr("Dividend %");

  auto transactions = ledgerModel->getTransactions();

  for (auto &it : transactions) {
    auto transaction = it.second;
    auto record = DividendRecord(
      it.first,
      transaction.GetBlockTime(),
      transaction.GetDividendCredit(),
      transaction.GetCoinSupply(),
      transaction.GetPayoutModifier(),
      transaction.GetHeight()
    );

    cachedLedger.append(record);
  }
}

DividendTableModel::~DividendTableModel() {

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
          return formatMoneySupply(rec,ChratosUnits::separatorAlways);
        case Percentage:
          return formatPercentage(rec);
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
