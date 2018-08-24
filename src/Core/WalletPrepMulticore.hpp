// Copyright (c) 2012-2018, The CryptoNote developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

// Copyright (c) 2018-2019, The Naza developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#pragma once

#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>
#include "CryptoNote.hpp"
#include "rpc_api.hpp"


namespace nazacoin {

class IBlockChainState;  
class Currency;


struct PreparedWalletTransaction {
	TransactionPrefix tx;
	KeyDerivation derivation;
	std::vector<PublicKey> spend_keys;

	PreparedWalletTransaction() = default;
	PreparedWalletTransaction(TransactionPrefix &&tx, const SecretKey &view_secret_key);
};

struct PreparedWalletBlock {
	BlockTemplate header;
	PreparedWalletTransaction base_transaction;
	Hash base_transaction_hash;
	std::vector<PreparedWalletTransaction> transactions;
	PreparedWalletBlock() = default;
	PreparedWalletBlock(BlockTemplate &&bc_header, std::vector<TransactionPrefix> &&raw_transactions,
	    Hash base_transaction_hash, const SecretKey &view_secret_key);
};

class WalletPreparatorMulticore {
	std::vector<std::thread> threads;
	std::mutex mu;
	std::condition_variable have_work;
	std::condition_variable result_ready;
	bool quit = false;

	std::map<Height, PreparedWalletBlock> prepared_blocks;
	api::nazad::SyncBlocks::Response work;
	int work_counter = 0;
	SecretKey work_secret_key;
	void thread_run();

public:
	WalletPreparatorMulticore();
	~WalletPreparatorMulticore();
	void cancel_work();
	void start_work(const api::nazad::SyncBlocks::Response &new_work, const SecretKey &view_secret_key);
	PreparedWalletBlock get_ready_work(Height height);
};

}  
