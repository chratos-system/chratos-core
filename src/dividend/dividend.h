// Copyright (c) 2018 The Chratos Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#ifndef CHRATOS_DIVIDEND_DIVIDEND_H
#define CHRATOS_DIVIDEND_DIVIDEND_H

#include "dividend/dividendledger.h"
#include "script/script.h"

class CDividend {
  public:
    static void SetScriptForDividendContribution(CScript &script);
    static CAmount GetDividendPayout(CAmount amount, int blockHeight);
    static CAmount GetTotalWithDividend(CAmount amount, int blockHeight);
};

#endif
