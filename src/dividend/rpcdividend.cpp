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

void ListDividendTransactions(const CDividendTx& dtx, UniValue& ret) {

  CAmount credit = dtx.GetAvailableCredit();

  UniValue entry(UniValue::VOBJ);

  DividendTxToJSON(dtx, entry);
  entry.push_back(Pair("amount", ValueFromAmount(credit)));
  ret.push_back(entry);
}

UniValue dividendgetbalance(const JSONRPCRequest &request) {
  if (!EnsureLedgerIsAvailable(request.fHelp)) {
    return NullUniValue;
  }

  if (request.fHelp || request.params.size() > 0) {
    throw runtime_error(
      "dividendgetbalance\n"
      "\nThe total amount burned in the dividend ledger\n"
    );
  }

  LOCK2(cs_main, pledgerMain->cs_ledger);

  return ValueFromAmount(pledgerMain->GetBalance());
}

UniValue dividendlisttransactions(const JSONRPCRequest &request) {
  if (!EnsureLedgerIsAvailable(request.fHelp)) {
    return NullUniValue;
  }

  if (request.fHelp || request.params.size() > 1) {
    throw runtime_error(
      "dividendlisttransactions ( count from )\n"
      "\nReturns up to 'count' most recent transactions skipping the first 'from' transactions for the dividend ledger'.\n"
      "\nArguments:\n"
      "1. count    (numeric, optional, default=10) The number of transactions to return\n"
      "2. from     (numeric, optional, default=0) The number of transactions to skip\n"
      "\nResult:\n"
      "[\n"
      "  {\n"
      "    \"address\":\"chratosaddress\",    (string) The chratos address of the transaction.\n"
      "    \"amount\": x.xxx,          (numeric) The amount in " + CURRENCY_UNIT + ".\n"
      "    \"vout\": n,                (numeric) the vout value\n"
      "    \"confirmations\": n,       (numeric) The number of confirmations for the transaction.\n"
      "    \"blockhash\": \"hashvalue\", (string) The block hash containing the transaction.\n"
      "    \"blockindex\": n,          (numeric) The index of the transaction in the block that includes it."
      "    \"blocktime\": xxx,         (numeric) The block time in seconds since epoch (1 Jan 1970 GMT).\n"
      "    \"txid\": \"transactionid\", (string) The transaction id.\n"
      "  }\n"
      "]\n"
   );
  }

  LOCK2(cs_main, pledgerMain->cs_ledger);

  int nCount = 10;
  if (request.params.size() > 0) {
    nCount = request.params[0].get_int();
  }

  int nFrom = 0;
  if (request.params.size() > 1) {
    nFrom = request.params[1].get_int();
  }

  UniValue ret(UniValue::VARR);

  const CDividendLedger::TxItems &txOrdered = pledgerMain->GetOrdered();

  for (CDividendLedger::TxItems::const_reverse_iterator it = txOrdered.rbegin();
      it != txOrdered.rend(); ++it) {

    CDividendTx *const pdtx = (*it).second;
    if (pdtx != 0) {
      ListDividendTransactions(*pdtx, ret);
    }
  }
 
  if (nFrom > (int)ret.size()) {
        nFrom = ret.size();
  }
  if ((nFrom + nCount) > (int)ret.size()) {
    nCount = ret.size() - nFrom;
  }

  vector<UniValue> arrTmp = ret.getValues();

  vector<UniValue>::iterator first = arrTmp.begin();
  std::advance(first, nFrom);
  vector<UniValue>::iterator last = arrTmp.begin();
  std::advance(last, nFrom+nCount);

  if (last != arrTmp.end()) arrTmp.erase(last, arrTmp.end());
  if (first != arrTmp.begin()) arrTmp.erase(arrTmp.begin(), first);

  std::reverse(arrTmp.begin(), arrTmp.end()); // Return oldest to newest

  ret.clear();
  ret.setArray();
  ret.push_backV(arrTmp);

  return ret;

}

static const CRPCCommand commands[] = {
  // Categoy    Name                  function     safe   argNames
  { "dividend", "dividendgetbalance", &dividendgetbalance, false, {} },
  { "dividend", "dividendlisttransactions", &dividendlisttransactions, false, {} }
};

void RegisterDividendRPCCommands(CRPCTable &tableRPC) {
  for (unsigned int vcidx = 0; vcidx < ARRAYLEN(commands); vcidx++) {
    tableRPC.appendCommand(commands[vcidx].name, &commands[vcidx]);
  }
}
