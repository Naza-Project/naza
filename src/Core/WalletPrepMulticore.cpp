// Copyright (c) 2012-2018, The CryptoNote developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

// Copyright (c) 2018-2019, The Naza developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#include "BlockChainState.hpp"
#include "Currency.hpp"
#include "WalletPrepMulticore.hpp"
#include "TransactionExtra.hpp"
#include "crypto/crypto.hpp"

using namespace nazacoin;



WalletPreparatorMulticore::WalletPreparatorMulticore() {
	auto th_count = std::max<size_t>(2, 3 * std::thread::hardware_concurrency() / 4);
	for (auto i = 0; i != th_count; ++i)
		threads.emplace_back(&WalletPreparatorMulticore::thread_run, this);
}

WalletPreparatorMulticore::~WalletPreparatorMulticore() {
	{
		std::unique_lock<std::mutex> lock(mu);
		quit = true;
		have_work.notify_all();
	}
	for (auto &&th : threads)
		th.join();
}

PreparedWalletTransaction::PreparedWalletTransaction(TransactionPrefix &&ttx, const SecretKey &view_secret_key)
    : tx(std::move(ttx)) {
	PublicKey tx_public_key = get_transaction_public_key_from_extra(tx.extra);
	if (!generate_key_derivation(tx_public_key, view_secret_key, derivation))
		return;
	KeyPair tx_keys;
	size_t key_index   = 0;
	uint32_t out_index = 0;
	spend_keys.reserve(tx.outputs.size());
	for (const auto &output : tx.outputs) {
		if (output.target.type() == typeid(KeyOutput)) {
			const KeyOutput &key_output = boost::get<KeyOutput>(output.target);
			PublicKey spend_key;
			underive_public_key(derivation, key_index, key_output.key,
			    spend_key);  
			spend_keys.push_back(spend_key);
			++key_index;
		}
		++out_index;
	}
}
PreparedWalletBlock::PreparedWalletBlock(BlockTemplate &&bc_header, std::vector<TransactionPrefix> &&raw_transactions,
    Hash base_transaction_hash, const SecretKey &view_secret_key)
    : base_transaction_hash(base_transaction_hash) {
	header           = bc_header;
	base_transaction = PreparedWalletTransaction(std::move(bc_header.base_transaction), view_secret_key);
	transactions.reserve(raw_transactions.size());
	for (auto tx_index = 0; tx_index != raw_transactions.size(); ++tx_index) {
		transactions.emplace_back(std::move(raw_transactions.at(tx_index)), view_secret_key);
	}
}

void WalletPreparatorMulticore::thread_run() {
	while (true) {
		SecretKey view_secret_key;
		Height height          = 0;
		int local_work_counter = 0;
		api::nazad::GetRawBlock::Response sync_block;
		std::vector<std::vector<uint32_t>> global_indices;
		{
			std::unique_lock<std::mutex> lock(mu);
			if (quit)
				return;
			if (work.blocks.empty()) {
				have_work.wait(lock);
				continue;
			}
			local_work_counter = work_counter;
			view_secret_key    = work_secret_key;
			height             = work.start_height;
			sync_block         = std::move(work.blocks.front());
			work.start_height += 1;
			work.blocks.erase(work.blocks.begin());
		}
		PreparedWalletBlock result(std::move(sync_block.raw_header), std::move(sync_block.raw_transactions),
		    sync_block.base_transaction_hash, view_secret_key);
		{
			std::unique_lock<std::mutex> lock(mu);
			if (local_work_counter == work_counter) {
				prepared_blocks[height] = std::move(result);
				result_ready.notify_all();
			}
		}
	}
}

void WalletPreparatorMulticore::cancel_work() {
	std::unique_lock<std::mutex> lock(mu);
	work = api::nazad::SyncBlocks::Response();
	prepared_blocks.clear();
	work_counter += 1;
}

void WalletPreparatorMulticore::start_work(const api::nazad::SyncBlocks::Response &new_work,
    const SecretKey &view_secret_key) {
	std::unique_lock<std::mutex> lock(mu);
	work            = new_work;
	work_secret_key = view_secret_key;
	work_counter += 1;
	have_work.notify_all();
}

PreparedWalletBlock WalletPreparatorMulticore::get_ready_work(Height height) {
	while (true) {
		std::unique_lock<std::mutex> lock(mu);
		auto pit = prepared_blocks.find(height);
		if (pit == prepared_blocks.end()) {
			result_ready.wait(lock);
			continue;
		}
		PreparedWalletBlock result = std::move(pit->second);
		pit                        = prepared_blocks.erase(pit);
		return result;
	}
}
