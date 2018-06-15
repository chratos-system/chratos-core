// Copyright (c) 2018 The Chratos Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "amount.h"
#include "init.h"
#include "main.h"
#include "dividend.h"
#include "dividenddb.h"
#include "dividendtx.h"
#include "rpc/server.h"
#include "utilmoneystr.h"

bool EnsureLedgerIsAvailable(bool avoidException) {
  if (!pledgerMain) {
    if (!avoidException) {
      throw JSONRPCError(RPC_METHOD_NOT_FOUND, "Method not found (disabled)");
    } else {
      return false;
    }
  }
  return true;
}

void DividendTxToJSON(const CDividendTx &dtx, UniValue &entry) {
  int confirms = dtx.GetDepthInMainChain();
  entry.push_back(Pair("confirmations", confirms));
  if (confirms > 0) {
    entry.push_back(Pair("blockhash", dtx.hashBlock.GetHex()));
    entry.push_back(Pair("blockindex", dtx.nIndex));
    entry.push_back(Pair("blocktime", mapBlockIndex[dtx.hashBlock]->GetBlockTime()));
  }

  uint256 hash = dtx.GetHash();
  entry.push_back(Pair("txid", hash.GetHex()));
  entry.push_back(Pair("time", dtx.GetBlockTime()));
}

UniValue getbalance(const JSONRPCRequest &request) {
  if (!EnsureLedgerIsAvailable(request.fHelp)) {
    return NullUniValue;
  }

  if (request.fHelp || request.params.size() > 0) {
    throw runtime_error(
      "getdividendbalance\n"
      "\nThe total amount in the dividend wallet\n"
    );
  }

  LOCK2(cs_main, pledgerMain->cs_ledger);

  return ValueFromAmount(pledgerMain->GetBalance());
}

static const CRPCCommand commands[] = {
  // Categoy    Name                  function     safe   argNames
  { "dividend", "getdividendbalance", &getbalance, false, {} }
};

void RegisterDividendRPCCommands(CRPCTable &tableRPC) {
  for (unsigned int vcidx = 0; vcidx < ARRAYLEN(commands); vcidx++) {
    tableRPC.appendCommand(commands[vcidx].name, &commands[vcidx]);
  }
}
