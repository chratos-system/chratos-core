// Copyright (c) 2018 The Chratos Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "dividendview.h"

#include "addresstablemodel.h"
#include "chratosunits.h"
#include "csvmodelwriter.h"
#include "editaddressdialog.h"
#include "guiutil.h"
#include "optionsmodel.h"
#include "platformstyle.h"
#include "transactiondescdialog.h"
#include "transactionfilterproxy.h"
#include "transactionrecord.h"
#include "transactiontablemodel.h"
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
) : QWidget(parent), model(0), dividendView(0) {

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

    QTableView *view = new QTableView(this);
    vlayout->addLayout(hlayout);
    vlayout->addWidget(createDateRangeWidget());
    vlayout->addWidget(view);
    vlayout->setSpacing(0);
    int width = view->verticalScrollBar()->sizeHint().width();
    // Cover scroll bar width with spacing
    if (platformStyle->getUseExtraSpacing()) {
        hlayout->addSpacing(width+2);
    } else {
        hlayout->addSpacing(width);
    }
    // Always show scroll bar
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    view->setTabKeyNavigation(false);
    view->setContextMenuPolicy(Qt::CustomContextMenu);

    view->installEventFilter(this);

    dividendView = view;
}

void DividendView::setModel(DividendLedgerModel *model) {
  this->model = model;
  if (model) {
    int unit = model->getOptionsModel()->getDisplayUnit();
    auto balance = model->getTotalDividendFund();
    amountLabel->setText(ChratosUnits::formatWithUnit(
          unit, balance, false, ChratosUnits::separatorAlways));
    /*
    transactionProxyModel = new DividendFilterProxy(this);
    transactionProxyModel->setSourceModel(model->getDividendTableModel());
    transactionProxyModel->setDynamicSortFilter(true);
    transactionProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    transactionProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    transactionProxyModel->setSortRole(Qt::EditRole);

    dividendView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    dividendView->setModel(transactionProxyModel);
    dividendView->setAlternatingRowColors(true);
    dividendView->setSelectionBehavior(QAbstractItemView::SelectRows);
    dividendView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    dividendView->setSortingEnabled(true);
    dividendView->sortByColumn(TransactionTableModel::Date, Qt::DescendingOrder);
    dividendView->verticalHeader()->hide();

    dividendView->setColumnWidth(TransactionTableModel::Status, STATUS_COLUMN_WIDTH);
    dividendView->setColumnWidth(TransactionTableModel::Date, DATE_COLUMN_WIDTH);
    dividendView->setColumnWidth(TransactionTableModel::Type, TYPE_COLUMN_WIDTH);
    dividendView->setColumnWidth(TransactionTableModel::Amount, AMOUNT_MINIMUM_COLUMN_WIDTH);

    columnResizingFixer = new GUIUtil::TableViewLastColumnResizingFixer(dividendView, AMOUNT_MINIMUM_COLUMN_WIDTH, MINIMUM_COLUMN_WIDTH, this);

    if (model->getOptionsModel()) {
      // Add third party transaction URLs to context menu
      QStringList listUrls = model->getOptionsModel()->getThirdPartyTxUrls().split("|", QString::SkipEmptyParts);
      for (int i = 0; i < listUrls.size(); ++i) {
        QString host = QUrl(listUrls[i].trimmed(), QUrl::StrictMode).host();
        if (!host.isEmpty())
        {
          QAction *thirdPartyTxUrlAction = new QAction(host, this); // use host as menu item label
          if (i == 0)
            contextMenu->addSeparator();
          contextMenu->addAction(thirdPartyTxUrlAction);
          connect(thirdPartyTxUrlAction, SIGNAL(triggered()), mapperThirdPartyTxUrls, SLOT(map()));
          mapperThirdPartyTxUrls->setMapping(thirdPartyTxUrlAction, listUrls[i].trimmed());
        }
      }
    }
    */
  }
}
/*
void DividendView::chooseDate(int idx)
{
    if(!transactionProxyModel)
        return;
    QDate current = QDate::currentDate();
    dateRangeWidget->setVisible(false);
    switch(dateWidget->itemData(idx).toInt())
    {
    case All:
        transactionProxyModel->setDateRange(
                TransactionFilterProxy::MIN_DATE,
                TransactionFilterProxy::MAX_DATE);
        break;
    case Today:
        transactionProxyModel->setDateRange(
                QDateTime(current),
                TransactionFilterProxy::MAX_DATE);
        break;
    case ThisWeek: {
        // Find last Monday
        QDate startOfWeek = current.addDays(-(current.dayOfWeek()-1));
        transactionProxyModel->setDateRange(
                QDateTime(startOfWeek),
                TransactionFilterProxy::MAX_DATE);

        } break;
    case ThisMonth:
        transactionProxyModel->setDateRange(
                QDateTime(QDate(current.year(), current.month(), 1)),
                TransactionFilterProxy::MAX_DATE);
        break;
    case LastMonth:
        transactionProxyModel->setDateRange(
                QDateTime(QDate(current.year(), current.month(), 1).addMonths(-1)),
                QDateTime(QDate(current.year(), current.month(), 1)));
        break;
    case ThisYear:
        transactionProxyModel->setDateRange(
                QDateTime(QDate(current.year(), 1, 1)),
                TransactionFilterProxy::MAX_DATE);
        break;
    case Range:
        dateRangeWidget->setVisible(true);
        dateRangeChanged();
        break;
    }
}

void DividendView::chooseType(int idx)
{
    if(!transactionProxyModel)
        return;
    transactionProxyModel->setTypeFilter(
        typeWidget->itemData(idx).toInt());
}

void DividendView::changedPrefix(const QString &prefix)
{
    if(!transactionProxyModel)
        return;
    transactionProxyModel->setAddressPrefix(prefix);
}

void DividendView::changedAmount(const QString &amount)
{
    if(!transactionProxyModel)
        return;
    CAmount amount_parsed = 0;
    if(ChratosUnits::parse(model->getOptionsModel()->getDisplayUnit(), amount, &amount_parsed))
    {
        transactionProxyModel->setMinAmount(amount_parsed);
    }
    else
    {
        transactionProxyModel->setMinAmount(0);
    }
}

void DividendView::exportClicked()
{
    // CSV is currently the only supported format
    QString filename = GUIUtil::getSaveFileName(this,
        tr("Export Transaction History"), QString(),
        tr("Comma separated file (*.csv)"), NULL);

    if (filename.isNull())
        return;

    CSVModelWriter writer(filename);

    // name, column, role
    writer.setModel(transactionProxyModel);
    writer.addColumn(tr("Confirmed"), 0, TransactionTableModel::ConfirmedRole);
    writer.addColumn(tr("Date"), 0, TransactionTableModel::DateRole);
    writer.addColumn(tr("Type"), TransactionTableModel::Type, Qt::EditRole);
    writer.addColumn(tr("Label"), 0, TransactionTableModel::LabelRole);
    writer.addColumn(tr("Address"), 0, TransactionTableModel::AddressRole);
    writer.addColumn(ChratosUnits::getAmountColumnTitle(model->getOptionsModel()->getDisplayUnit()), 0, TransactionTableModel::FormattedAmountRole);
    writer.addColumn(tr("ID"), 0, TransactionTableModel::TxIDRole);

    if(!writer.write()) {
        Q_EMIT message(tr("Exporting Failed"), tr("There was an error trying to save the transaction history to %1.").arg(filename),
            CClientUIInterface::MSG_ERROR);
    }
    else {
        Q_EMIT message(tr("Exporting Successful"), tr("The transaction history was successfully saved to %1.").arg(filename),
            CClientUIInterface::MSG_INFORMATION);
    }
}

void DividendView::contextualMenu(const QPoint &point)
{
    QModelIndex index = dividendView->indexAt(point);
    QModelIndexList selection = dividendView->selectionModel()->selectedRows(0);
    if (selection.empty()) {
        return;
    }

    uint256 hash;
    hash.SetHex(selection.at(0).data(TransactionTableModel::TxHashRole).toString().toStdString());

    if(index.isValid())
    {
        contextMenu->exec(QCursor::pos());
    }
}

void DividendView::copyAddress()
{
    GUIUtil::copyEntryData(dividendView, 0, TransactionTableModel::AddressRole);
}

void DividendView::copyLabel()
{
    GUIUtil::copyEntryData(dividendView, 0, TransactionTableModel::LabelRole);
}

void DividendView::copyAmount()
{
    GUIUtil::copyEntryData(dividendView, 0, TransactionTableModel::FormattedAmountRole);
}

void DividendView::copyTxID()
{
    GUIUtil::copyEntryData(dividendView, 0, TransactionTableModel::TxIDRole);
}

void DividendView::copyTxHex()
{
    GUIUtil::copyEntryData(dividendView, 0, TransactionTableModel::TxHexRole);
}

void DividendView::copyTxPlainText()
{
    GUIUtil::copyEntryData(dividendView, 0, TransactionTableModel::TxPlainTextRole);
}

void DividendView::showDetails() {
  if(!dividendView->selectionModel()) {
    return;
  }

  QModelIndexList selection = dividendView->selectionModel()->selectedRows();

  if(!selection.isEmpty()) {
    TransactionDescDialog *dlg = new TransactionDescDialog(selection.at(0));
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
  }
}

void DividendView::openThirdPartyTxUrl(QString url)
{
    if(!dividendView || !dividendView->selectionModel())
        return;
    QModelIndexList selection = dividendView->selectionModel()->selectedRows(0);
    if(!selection.isEmpty())
         QDesktopServices::openUrl(QUrl::fromUserInput(url.replace("%s", selection.at(0).data(TransactionTableModel::TxHashRole).toString())));
}
*/
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
/*
void DividendView::focusTransaction(const QModelIndex &idx)
{
    if(!transactionProxyModel)
        return;
    QModelIndex targetIdx = transactionProxyModel->mapFromSource(idx);
    dividendView->scrollTo(targetIdx);
    dividendView->setCurrentIndex(targetIdx);
    dividendView->setFocus();
}

// We override the virtual resizeEvent of the QWidget to adjust tables column
// sizes as the tables width is proportional to the dialogs width.
void DividendView::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    columnResizingFixer->stretchColumnWidth(TransactionTableModel::ToAddress);
}

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
