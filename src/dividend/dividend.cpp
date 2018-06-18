// Copyright (c) 2018 The Chratos Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "dividend/dividend.h"

void CDividend::SetScriptForDividendContribution(CScript &script) {
    script.resize(2);
    script[0] = OP_RETURN;
    script[1] = OP_DIVIDEND;
}
