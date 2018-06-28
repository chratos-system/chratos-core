// Copyright (c) 2018 The Chratos Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef CHRATOS_QT_DIVIDENDMODEL_H
#define CHRATOS_QT_DIVIDENDMODEL_H

#include "dividend/dividendledger.h"

#include <QObject>

class OptionsModel;
class DividendTableModel;
class PlatformStyle;

class DividendLedgerModel : public QObject {
  public:
    explicit DividendLedgerModel(
      const PlatformStyle *platformStyle,
      CDividendLedger *ledger, 
      OptionsModel *optionsModel,
      QObject *parent = 0
    );

//    DividendTableModel *getDividendTableModel();

    CAmount getTotalDividendFund() const;

    OptionsModel *getOptionsModel();

  private:
    CDividendLedger *ledger;

    OptionsModel *optionsModel;

    //DividendTableModel *dividendTableModel;
};

#endif
