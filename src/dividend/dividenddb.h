// Copyright (c) 2018 The Chratos Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#ifndef CHRATOS_DIVIDEND_DIVIDEND_DB_H
#define CHRATOS_DIVIDEND_DIVIDEND_DB_H

#include "dividend/dbdiv.h"
//#include "wallet/db.h"
#include "database/dberrors.h"
#include "primitives/block.h"

class CDividendLedger;
class CDividendTx;

static const bool DEFAULT_FLUSHLEDGER = true;

class CDividendLedgerDB : public CDBDiv {
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

    bool ReadBestBlock(CBlockLocator& locator);
    bool WriteBestBlock(const CBlockLocator& locator);
};

void ThreadFlushLedgerDB(std::string strFile);

#endif

