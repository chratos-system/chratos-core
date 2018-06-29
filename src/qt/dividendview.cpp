// Copyright (c) 2018 The Chratos Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "dividendview.h"

#include "chratosunits.h"
#include "csvmodelwriter.h"
#include "guiutil.h"
#include "optionsmodel.h"
#include "platformstyle.h"
#include "dividendtablemodel.h"
#include "dividendledgermodel.h"

#include "ui_interface.h"

#include <QComboBox>
#include <QDateTimeEdit>
#include <QDesktopServices>
#include <QDoubleValidator>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPoint>
#include <QScrollBar>
#include <QSignalMapper>
#include <QTableView>
#include <QUrl>
#include <QVBoxLayout>

DividendView::DividendView(
  const PlatformStyle *platformStyle, QWidget *parent
) : QWidget(parent), model(0), tableView(0), platformStyle(platformStyle) {

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(0,0,0,6);
    // Build filter row
    setContentsMargins(0,0,0,0);


    QFont font1;
    font1.setPointSize(16);
    font1.setBold(false);
    font1.setWeight(50);
    font1.setStyleStrategy(QFont::PreferAntialias);

    titleLabel = new QLabel(this);
    titleLabel->setText("Total Paid Out:");
    titleLabel->setFont(font1);
    hlayout->addWidget(titleLabel);

    amountLabel = new QLabel(this);
    amountLabel->setFont(font1);
    hlayout->addWidget(amountLabel);

    if (platformStyle->getUseExtraSpacing()) {
        hlayout->setSpacing(10);
        hlayout->addSpacing(26);
    } else {
        hlayout->setSpacing(5);
        hlayout->addSpacing(23);
    }

    dateWidget = new QComboBox(this);
    if (platformStyle->getUseExtraSpacing()) {
        dateWidget->setFixedWidth(121);
    } else {
        dateWidget->setFixedWidth(120);
    }
    dateWidget->addItem(tr("All"), All);
    dateWidget->addItem(tr("Today"), Today);
    dateWidget->addItem(tr("This week"), ThisWeek);
    dateWidget->addItem(tr("This month"), ThisMonth);
    dateWidget->addItem(tr("Last month"), LastMonth);
    dateWidget->addItem(tr("This year"), ThisYear);
    dateWidget->addItem(tr("Range..."), Range);
    hlayout->addWidget(dateWidget);

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setContentsMargins(0,0,0,0);
    vlayout->setSpacing(0);

    QTableView *tableView = new QTableView(this);
    vlayout->addLayout(hlayout);
    vlayout->addWidget(createDateRangeWidget());
    vlayout->addWidget(tableView);
    vlayout->setSpacing(0);
    int width = tableView->verticalScrollBar()->sizeHint().width();
    // Cover scroll bar width with spacing
    if (platformStyle->getUseExtraSpacing()) {
        hlayout->addSpacing(width+2);
    } else {
        hlayout->addSpacing(width);
    }
    // Always show scroll bar
    tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    tableView->setTabKeyNavigation(false);
    tableView->setContextMenuPolicy(Qt::CustomContextMenu);

    tableView->installEventFilter(this);

    this->tableView = tableView;
}

void DividendView::setModel(DividendLedgerModel *model) {
  this->model = model;
  if (model) {
    tableModel = new DividendTableModel(platformStyle, model, this);
    int unit = model->getOptionsModel()->getDisplayUnit();
    auto balance = model->getTotalDividendFund();
    amountLabel->setText(ChratosUnits::formatWithUnit(
          unit, balance, false, ChratosUnits::separatorAlways));

    /*
    tableModel->setDynamicSortFilter(true);
    tableModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    tableModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    tableModel->setSortRole(Qt::EditRole);
    */

    tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableView->setModel(tableModel);
    tableView->setAlternatingRowColors(true);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tableView->setSortingEnabled(true);
    tableView->sortByColumn(DividendTableModel::Date, Qt::DescendingOrder);
    tableView->verticalHeader()->hide();

    tableView->setColumnWidth(
      DividendTableModel::Date, DATE_COLUMN_WIDTH);
    tableView->setColumnWidth(
      DividendTableModel::Amount, AMOUNT_COLUMN_WIDTH);
    tableView->setColumnWidth(
      DividendTableModel::BlockHeight, BLOCK_HEIGHT_COLUMN_WIDTH);
    tableView->setColumnWidth(
      DividendTableModel::TransactionID, TXID_COLUMN_WIDTH);
    tableView->setColumnWidth(
      DividendTableModel::MoneySupply, MONEY_SUPPLY_COLUMN_WIDTH);
    tableView->setColumnWidth(
      DividendTableModel::Percentage, PERCENTAGE_COLUMN_WIDTH);

    columnResizingFixer = new GUIUtil::TableViewLastColumnResizingFixer(
      tableView, AMOUNT_MINIMUM_COLUMN_WIDTH, MINIMUM_COLUMN_WIDTH, this
    );
  }
}

QWidget *DividendView::createDateRangeWidget()
{
    dateRangeWidget = new QFrame();
    dateRangeWidget->setFrameStyle(QFrame::Panel | QFrame::Raised);
    dateRangeWidget->setContentsMargins(1,1,1,1);
    QHBoxLayout *layout = new QHBoxLayout(dateRangeWidget);
    layout->setContentsMargins(0,0,0,0);
    layout->addSpacing(23);
    layout->addWidget(new QLabel(tr("Range:")));

    dateFrom = new QDateTimeEdit(this);
    dateFrom->setDisplayFormat("dd/MM/yy");
    dateFrom->setCalendarPopup(true);
    dateFrom->setMinimumWidth(100);
    dateFrom->setDate(QDate::currentDate().addDays(-7));
    layout->addWidget(dateFrom);
    layout->addWidget(new QLabel(tr("to")));

    dateTo = new QDateTimeEdit(this);
    dateTo->setDisplayFormat("dd/MM/yy");
    dateTo->setCalendarPopup(true);
    dateTo->setMinimumWidth(100);
    dateTo->setDate(QDate::currentDate());
    layout->addWidget(dateTo);
    layout->addStretch();

    // Hide by default
    dateRangeWidget->setVisible(false);

    // Notify on change
    connect(dateFrom, SIGNAL(dateChanged(QDate)), this, SLOT(dateRangeChanged()));
    connect(dateTo, SIGNAL(dateChanged(QDate)), this, SLOT(dateRangeChanged()));

    return dateRangeWidget;
}

void DividendView::dateRangeChanged() {
}

// We override the virtual resizeEvent of the QWidget to adjust tables column
// sizes as the tables width is proportional to the dialogs width.
void DividendView::resizeEvent(QResizeEvent* event){
  QWidget::resizeEvent(event);
  columnResizingFixer->stretchColumnWidth(DividendTableModel::TransactionID);
}
/*
// Need to override default Ctrl+C action for amount as default behaviour is just to copy DisplayRole text
bool DividendView::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_C && ke->modifiers().testFlag(Qt::ControlModifier))
        {
             GUIUtil::copyEntryData(dividendView, 0, TransactionTableModel::TxPlainTextRole);
             return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}
*/
