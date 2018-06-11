// Copyright (c) 2017-2018 The Chratos Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef CHRATOS_POS_H
#define CHRATOS_POS_H

#define COMMUNITY_FUND_AMOUNT 25000000

static const int STAKE_TIMESTAMP_MASK = 15;

double GetDifficulty(const CBlockIndex* blockindex);

double GetPoWMHashPS();

double GetPoSKernelPS();

extern uint64_t nLastCoinStakeSearchInterval;

int64_t GetProofOfStakeReward(int nHeight, int64_t nCoinAge, int64_t nFees, CBlockIndex* pindexPrev);

#endif // CHRATOS_POS_H
