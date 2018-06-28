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

    /*
    enum ColumnWidths {
        STATUS_COLUMN_WIDTH = 30,
        DATE_COLUMN_WIDTH = 120,
        TYPE_COLUMN_WIDTH = 113,
        AMOUNT_MINIMUM_COLUMN_WIDTH = 150,
        MINIMUM_COLUMN_WIDTH = 23
    };
    */

private:
    DividendLedgerModel *model;

    QFrame *dateRangeWidget;

    QComboBox *dateWidget;

    QWidget *createDateRangeWidget();

    QDateTimeEdit *dateFrom;

    QDateTimeEdit *dateTo;

    QTableView *dividendView;

    QLabel *amountLabel;

    QLabel *titleLabel;

    /*
    TransactionFilterProxy *transactionProxyModel;

    QComboBox *typeWidget;
    QLineEdit *addressWidget;
    QLineEdit *amountWidget;

    QMenu *contextMenu;
    QSignalMapper *mapperThirdPartyTxUrls;



    GUIUtil::TableViewLastColumnResizingFixer *columnResizingFixer;

    virtual void resizeEvent(QResizeEvent* event);

    bool eventFilter(QObject *obj, QEvent *event);
    */

private Q_SLOTS:
    void dateRangeChanged();
  /*
    void contextualMenu(const QPoint &);
    void showDetails();
    void copyAddress();
    void editLabel();
    void copyLabel();
    void copyAmount();
    void copyTxID();
    void copyTxHex();
    void copyTxPlainText();
    void openThirdPartyTxUrl(QString url);

Q_SIGNALS:
    void doubleClicked(const QModelIndex&);
*/
    /**  Fired when a message should be reported to the user */
/*
    void message(const QString &title, const QString &message, unsigned int style);

public Q_SLOTS:
    void chooseDate(int idx);
    void chooseType(int idx);
    void changedPrefix(const QString &prefix);
    void changedAmount(const QString &amount);
    void exportClicked();
    void focusTransaction(const QModelIndex&);
*/
};


#endif
