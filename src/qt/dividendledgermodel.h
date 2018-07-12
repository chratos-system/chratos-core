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
class ChratosGUI;

class DividendLedgerModel : public QObject {
  
  Q_OBJECT

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

    void setChratosGUI(ChratosGUI *gui);

Q_SIGNALS:
    void incomingDividend(const QString &, int, const CAmount &);
  
  private:

    CDividendLedger *ledger;

    WalletModel *walletModel;

    OptionsModel *optionsModel;

    int cachedNumBlocks;

    bool fForceCheckDividendChanged;

    const PlatformStyle *platformStyle;

    void subscribeToCoreSignals();

    void unsubscribeFromCoreSignals();

    void updateDividend(
      const uint256 &hash, const ChangeType &status
    );

    void pollDividendChange();

    DividendTableModel *dividendTableModel;

};

#endif
