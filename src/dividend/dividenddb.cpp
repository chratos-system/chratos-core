// Copyright (c) 2018 The Chratos Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "dividend/dividenddb.h"
#include "dividend/dividend.h"
#include "dividend/dividendtx.h"
#include "consensus/validation.h"
#include "main.h"

struct CDividendLedgerScanState {
    bool fAnyUnordered;
    int nFileVersion;
    std::vector<uint256> vLedgerUpgrade;

    CDividendLedgerScanState() {
        fAnyUnordered = false;
        nFileVersion = 0;
    }
};


CDividendLedgerDB::CDividendLedgerDB(const std::string &filename,
                                     const char *pszMode,
                                     bool fFlushOnClose)
: CDB(filename, pszMode, fFlushOnClose), nLedgerDBUpdated(0) {
}

bool CDividendLedgerDB::WriteTx(const CDividendTx &dtx) {
  nLedgerDBUpdated++;
  return Write(std::make_pair(std::string("tx"), dtx.GetHash()), dtx);
}

bool CDividendLedgerDB::EraseTx(uint256 hash) {
  nLedgerDBUpdated++;
  return Erase(std::make_pair(std::string("tx"), hash));
}

bool
ReadKeyValue(CDividendLedger* pledger, CDataStream& ssKey, CDataStream& ssValue,
             CDividendLedgerScanState &lss, std::string& strType, std::string& strErr) {

  try {
    // Unserialize
    // Taking advantage of the fact that pair serialization
    // is just the two items serialized one after the other
    ssKey >> strType;
    if (strType == "tx") {
      uint256 hash;
      ssKey >> hash;
      CDividendTx dtx;
      ssValue >> dtx;
      CValidationState state;

      if (!(CheckTransaction(dtx, state) && (dtx.GetHash() == hash) && state.IsValid())) {
        return false;
      }

      //if (dtx.nOrderPos == -1) {
      //  lss.fAnyUnordered = true;
      //}

      pledger->AddToLedger(dtx, true, nullptr);
    } else if (strType == "version") {
      ssValue >> lss.nFileVersion;
    }
  } catch (...) {
  }

  return true;
}

DBErrors CDividendLedgerDB::LoadLedger(CDividendLedger *pledger) {
  bool fNoncriticalErrors = false;
  DBErrors result = DB_LOAD_OK;
  CDividendLedgerScanState lss;

  try {
    LOCK(pledger->cs_ledger);
    int nMinVersion = 0;
    if (Read((std::string)"minversion", nMinVersion)) {
      if (nMinVersion > CLIENT_VERSION) {
        return DB_TOO_NEW;
      }
      pledger->LoadMinVersion(nMinVersion);
    }

    // Get cursor
    Dbc* pcursor = GetCursor();
    if (!pcursor) {
      LogPrintf("Error getting ledger database cursor\n");
      return DB_CORRUPT;
    }

    while (true) {
      // Read next record
      CDataStream ssKey(SER_DISK, CLIENT_VERSION);
      CDataStream ssValue(SER_DISK, CLIENT_VERSION);
      int ret = ReadAtCursor(pcursor, ssKey, ssValue);
      if (ret == DB_NOTFOUND) {
        break;
      } else if (ret != 0) {
        LogPrintf("Error reading next record from ledger database\n");
        return DB_CORRUPT;
      }

      // Try to be tolerant of single corrupt records:
      std::string strType, strErr;
      if (!ReadKeyValue(pledger, ssKey, ssValue, lss, strType, strErr)) {
        fNoncriticalErrors = true; // ... but do warn the user there is something wrong.
        if (strType == "tx") {
          SoftSetBoolArg("-rescanledger", true);
        }
      }
      if (!strErr.empty()) {
        LogPrintf("%s\n", strErr);
      }
    }
    pcursor->close();
  } catch (const boost::thread_interrupted&) {
    throw;
  } catch (...) {
    result = DB_CORRUPT;
  }

  if (fNoncriticalErrors && result == DB_LOAD_OK) {
    result = DB_NONCRITICAL_ERROR;
  }

  // Any ledger corruption at all: skip any rewriting or
  // upgrading, we don't want to make it worse.
  if (result != DB_LOAD_OK) {
    return result;
  }

  LogPrintf("nFileVersion = %d\n", lss.nFileVersion);

  BOOST_FOREACH(uint256 hash, lss.vLedgerUpgrade) {

    WriteTx(pledger->GetMapLedger()[hash]);
  }

  // Rewrite encrypted ledger of versions 0.4.0 and 0.5.0rc:
  if (lss.nFileVersion < CLIENT_VERSION) // Update
    WriteVersion(CLIENT_VERSION);

  //if (lss.fAnyUnordered)
  //  result = ReorderTransactions(pledger);

  //pledger->laccentries.clear();
  //ListAccountCreditDebit("*", pledger->laccentries);
  //BOOST_FOREACH(CAccountingEntry& entry, pledger->laccentries) {
  //  pledger->wtxOrdered.insert(make_pair(entry.nOrderPos, CWallet::TxPair((CWalletTx*)0, &entry)));
  //}

  return result;
}

DBErrors CDividendLedgerDB::FindDividendTx(CDividendLedger *pledger,
                                          std::vector<uint256>& vTxHash,
                                          std::vector<CDividendTx> &vDtx) {

  bool fNoncriticalErrors = false;
  DBErrors result = DB_LOAD_OK;

  try {

    LOCK(pledger->cs_ledger);
    int nMinVersion = 0;
    if (Read((std::string)"minversion", nMinVersion)) {
      if (nMinVersion > CLIENT_VERSION) {
        return DB_TOO_NEW;
      }
      pledger->LoadMinVersion(nMinVersion);
    }

    // Get cursor
    Dbc* pcursor = GetCursor();
    if (!pcursor) {
      LogPrintf("Error getting ledger database cursor\n");
      return DB_CORRUPT;
    }

    while (true) {
      // Read next record
      CDataStream ssKey(SER_DISK, CLIENT_VERSION);
      CDataStream ssValue(SER_DISK, CLIENT_VERSION);
      int ret = ReadAtCursor(pcursor, ssKey, ssValue);
      if (ret == DB_NOTFOUND) {
        break;
      } else if (ret != 0) {
        LogPrintf("Error reading next record from ledger database\n");
        return DB_CORRUPT;
      }

      std::string strType;
      ssKey >> strType;
      if (strType == "tx") {
        uint256 hash;
        ssKey >> hash;

        CDividendTx dtx;
        ssValue >> dtx;

        vTxHash.push_back(hash);
        vDtx.push_back(dtx);
      }
    }
    pcursor->close();

  } catch (const boost::thread_interrupted&) {
    throw;
  } catch (...) {
    result = DB_CORRUPT;
  }

  return result;
}

bool CDividendLedgerDB::ReadBestBlock(CBlockLocator& locator) {
  if (Read(std::string("bestblock"), locator) && !locator.vHave.empty()) {
    return true;
  }
  return Read(std::string("bestblock_nomerkle"), locator);
}

bool CDividendLedgerDB::WriteBestBlock(const CBlockLocator& locator) {
  nWalletDBUpdated++;
  Write(std::string("bestblock"), CBlockLocator());
  return Write(std::string("bestblock_nomerkle"), locator);
}
