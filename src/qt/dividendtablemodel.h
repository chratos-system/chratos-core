// Copyright (c) 2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef CHRATOS_QT_DIVIDENDTABLEMODEL_H
#define CHRATOS_QT_DIVIDENDTABLEMODEL_H

#include "chratosunits.h"
#include "dividendrecord.h"
#include "dividend/dividendtx.h"
#include <QAbstractTableModel>
#include <QStringList>

class PlatformStyle;
class DividendLedgerModel;
class CDividendLedger;
class DividendView;

class DividendTableModel : public QAbstractTableModel {

  Q_OBJECT

  public:

    enum ColumnIndex {
      Date = 0,
      TransactionID = 1,
      BlockHeight = 2,
      Amount = 3,
      MoneySupply = 4,
      Received = 5
    };

    enum RoleIndex {
      /** Whole transaction as plain text **/
      TxPlainTextRole,
      TxHashRole,
      TxHexRole,
      TxIDRole
    };

    explicit DividendTableModel(
      const PlatformStyle *platformStyle, 
      DividendLedgerModel *model,
      CDividendLedger *ledger,
      DividendView *parent = 0
    );

    ~DividendTableModel();

    int rowCount(const QModelIndex &parent) const;

    int columnCount(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role) const;

    QVariant headerData(
      int section, Qt::Orientation orientation, int role
    ) const;

    QModelIndex index(
      int row, int column, const QModelIndex &parent = QModelIndex()
    ) const;

  public Q_SLOTS:

    void updateDividend(
      const QString &hash, int status, bool showTransaction
    );

    void updateConfirmations();

    void updateDisplayUnit();

    void updateAmountColumnTitle();

  private:

    DividendLedgerModel *ledgerModel;

    CDividendLedger *ledger;

    DividendView *dividendView;

    QStringList columns;

    const PlatformStyle *platformStyle;

    QList<DividendRecord> cachedLedger;

    void subscribeToCoreSignals();

    void unsubscribeFromCoreSignals();

    QString formatDivDate(const DividendRecord *rec) const;

    QString formatMoneySupply(
      const DividendRecord *rec,
      ChratosUnits::SeparatorStyle separators
    ) const;

    QString formatDivAmount(
      const DividendRecord *rec,
      ChratosUnits::SeparatorStyle separators
    ) const;

    QString formatReceived(
      const DividendRecord *rec,
      ChratosUnits::SeparatorStyle separators
    ) const;

    QString formatAmount(
      const CAmount &amount,
      ChratosUnits::SeparatorStyle separators
    ) const;

    QString formatDivHeight(const DividendRecord *rec) const;

    QString formatTxId(const DividendRecord *rec) const;

    QString formatPercentage(const DividendRecord *rec) const;

    void setupTransactions();

};

#endif
