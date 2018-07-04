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
class WalletModel;
class CDividendTx;

class DividendLedgerModel : public QObject {
  public:
    explicit DividendLedgerModel(
      const PlatformStyle *platformStyle,
      CDividendLedger *ledger, 
      WalletModel *walletModel,
      OptionsModel *optionsModel,
      QObject *parent = 0
    );

    int getCount() const;

    CAmount getTotalDividendFund() const;

    OptionsModel *getOptionsModel();
  
    std::map<uint256, CDividendTx> getTransactions() const;

    void setDividendTableModel(DividendTableModel *dividendTableModel);

    CDividendLedger *getLedger() const;

    CAmount getAmountReceived(const CDividendTx &tx) const;

  private:

    CDividendLedger *ledger;

    WalletModel *walletModel;

    OptionsModel *optionsModel;

    int cachedNumBlocks;

    bool fForceCheckDividendChanged;

    const PlatformStyle *platformStyle;

    void subscribeToCoreSignals();

    void unsubscribeFromCoreSignals();

    void updateDividend();

    void pollDividendChange();

    DividendTableModel *dividendTableModel;
};

#endif
