// Copyright (c) 2018 The Chratos Core developers
//
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef CHRATOS_QT_DIVIDENDVIEW_H
#define CHRATOS_QT_DIVIDENDVIEW_H

#include "guiutil.h"

#include <QWidget>
#include <QKeyEvent>

class PlatformStyle;
class TransactionFilterProxy;
class DividendLedgerModel;
class DividendTableModel;

QT_BEGIN_NAMESPACE
class QComboBox;
class QDateTimeEdit;
class QFrame;
class QLineEdit;
class QMenu;
class QModelIndex;
class QSignalMapper;
class QTableView;
QT_END_NAMESPACE

/** Widget showing the transaction list for a wallet, including a filter row.
    Using the filter row, the user can view or export a subset of the transactions.
  */
class DividendView : public QWidget
{
    Q_OBJECT

public:
    explicit DividendView(const PlatformStyle *platformStyle, QWidget *parent = 0);

    void setModel(DividendLedgerModel *model);

    // Date ranges for filter
    enum DateEnum
    {
        All,
        Today,
        ThisWeek,
        ThisMonth,
        LastMonth,
        ThisYear,
        Range
    };

    enum ColumnWidths {
      DATE_COLUMN_WIDTH = 120,
      AMOUNT_COLUMN_WIDTH = 150,
      BLOCK_HEIGHT_COLUMN_WIDTH = 90,
      TXID_COLUMN_WIDTH =  440,
      MONEY_SUPPLY_COLUMN_WIDTH = 150,
      PERCENTAGE_COLUMN_WIDTH = 80,
      AMOUNT_MINIMUM_COLUMN_WIDTH = 150,
      MINIMUM_COLUMN_WIDTH = 23
    };

private:
    DividendLedgerModel *model;

    QFrame *dateRangeWidget;

    QComboBox *dateWidget;

    QWidget *createDateRangeWidget();

    QDateTimeEdit *dateFrom;

    QDateTimeEdit *dateTo;

    QTableView *tableView;

    QLabel *amountLabel;

    QLabel *titleLabel;

    DividendTableModel *tableModel;

    const PlatformStyle *platformStyle;

    GUIUtil::TableViewLastColumnResizingFixer *columnResizingFixer;

    virtual void resizeEvent(QResizeEvent* event);

private Q_SLOTS:
    void dateRangeChanged();
};


#endif
