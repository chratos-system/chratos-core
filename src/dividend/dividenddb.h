// Copyright (c) 2018 The Chratos Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#ifndef CHRATOS_DIVIDEND_DIVIDEND_DB_H
#define CHRATOS_DIVIDEND_DIVIDEND_DB_H

#include "wallet/db.h"
#include "wallet/walletdb.h"

class CDividendLedger;
class CDividendTx;

class CDividendLedgerDB : public CDB {
  public:
    CDividendLedgerDB(const std::string &filename,
                      const char *pszMode = "r+",
                      bool fFlushOnClose = true);

    bool WriteTx(const CDividendTx &dTx);

    bool EraseTx(uint256 hash);

    DBErrors LoadLedger(CDividendLedger* pledger);

    DBErrors FindDividendTx(CDividendLedger* pledger,
                            std::vector<uint256>& vTxHash,
                            std::vector<CDividendTx>& vDtx);

  private:
    unsigned int nLedgerDBUpdated;
};

#endif

