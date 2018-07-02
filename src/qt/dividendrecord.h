// Copyright (c) 2018 The Chratos Core developers
//
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef CHRATOS_QT_DIVIDENDRECORD_H
#define CHRATOS_QT_DIVIDENDRECORD_H

#include "amount.h"
#include "uint256.h"

#include <QList>
#include <QString>

class CDividendTx;

class DividendRecord {
  public:
    DividendRecord(
      uint256 hash,
      qint64 time,
      const CAmount &amount,
      const CAmount &supply,
      double modifier,
      int64_t height
    );

    static DividendRecord fromDividendTx(const CDividendTx &tx);

    CAmount getAmount() const;

    CAmount getSupply() const;

    double getModifier() const;

    uint256 getHash() const;

    qint64 getTime() const;

    int64_t getBlockHeight() const;

  private:

    CAmount amount;

    CAmount supply;

    double modifier;

    uint256 hash;

    qint64 time;

    int64_t blockHeight;
};

#endif
