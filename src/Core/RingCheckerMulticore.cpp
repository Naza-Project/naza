// Copyright (c) 2012-2018, The CryptoNote developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

// Copyright (c) 2018-2019, The Naza developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#include "RingCheckerMulticore.hpp"
#include "BlockChainState.hpp"
#include "Currency.hpp"
#include "TransactionExtra.hpp"
#include "crypto/crypto.hpp"

using namespace nazacoin;

RingCheckerMulticore::RingCheckerMulticore() {
	auto th_count = std::max<size_t>(2, 3 * std::thread::hardware_concurrency() / 4);
	for (size_t i = 0; i != th_count; ++i)
		threads.emplace_back(&RingCheckerMulticore::thread_run, this);
}

RingCheckerMulticore::~RingCheckerMulticore() {
	{
		std::unique_lock<std::mutex> lock(mu);
		quit = true;
		have_work.notify_all();
	}
	for (auto &&th : threads)
		th.join();
}

void RingCheckerMulticore::thread_run() {
	while (true) {
		RingSignatureArg arg;
		int local_work_counter = 0;
		{
			std::unique_lock<std::mutex> lock(mu);
			if (quit)
				return;
			if (args.empty()) {
				have_work.wait(lock);
				continue;
			}
			local_work_counter = work_counter;
			arg                = std::move(args.front());
			args.pop_front();
		}
		std::vector<const PublicKey *> output_key_pointers;
		output_key_pointers.reserve(arg.output_keys.size());
		std::for_each(arg.output_keys.begin(), arg.output_keys.end(),
		    [&output_key_pointers](const PublicKey &key) { output_key_pointers.push_back(&key); });
		bool key_corrupted = false;
		bool result        = check_ring_signature(arg.tx_prefix_hash, arg.key_image, output_key_pointers.data(),
            output_key_pointers.size(), arg.signatures.data(), true, &key_corrupted);
		{
			std::unique_lock<std::mutex> lock(mu);
			if (local_work_counter == work_counter) {
				ready_counter += 1;
				if (!result && key_corrupted)  // TODO - db corrupted
					errors.push_back("INPUT_CORRUPTED_SIGNATURES");
				if (!result && !key_corrupted)
					errors.push_back("INPUT_INVALID_SIGNATURES");
				result_ready.notify_all();
			}
		}
	}
}
void RingCheckerMulticore::cancel_work() {
	std::unique_lock<std::mutex> lock(mu);
	args.clear();
	work_counter += 1;
}

std::string RingCheckerMulticore::start_work_get_error(IBlockChainState *state, const Currency &currency,
    const Block &block, Height unlock_height, Timestamp unlock_timestamp) {
	{
		std::unique_lock<std::mutex> lock(mu);
		args.clear();
		errors.clear();
		ready_counter = 0;
		work_counter += 1;
	}
	total_counter = 0;
	for (auto &&transaction : block.transactions) {
		auto tx_prefix_hash = get_transaction_prefix_hash(transaction);
		size_t input_index  = 0;
		for (const auto &input : transaction.inputs) {
			if (input.type() == typeid(CoinbaseInput)) {
			} else if (input.type() == typeid(KeyInput)) {
				const KeyInput &in = boost::get<KeyInput>(input);
				RingSignatureArg arg;
				arg.tx_prefix_hash = tx_prefix_hash;
				arg.key_image      = in.key_image;
				arg.signatures     = transaction.signatures[input_index];
				Height height      = 0;
				if (state->read_keyimage(in.key_image, &height))
					return "INPUT_KEYIMAGE_ALREADY_SPENT";
				if (in.output_indexes.empty())
					return "INPUT_UNKNOWN_TYPE";
				std::vector<uint32_t> global_indexes(in.output_indexes.size());
				global_indexes[0] = in.output_indexes[0];
				for (size_t i = 1; i < in.output_indexes.size(); ++i) {
					global_indexes[i] = global_indexes[i - 1] + in.output_indexes[i];
				}
				arg.output_keys.resize(global_indexes.size());
				for (size_t i = 0; i != global_indexes.size(); ++i) {
					IBlockChainState::UnlockTimePublickKeyHeightSpent unp;
					if (!state->read_amount_output(in.amount, global_indexes[i], &unp))
						return "INPUT_INVALID_GLOBAL_INDEX";
					if (!currency.is_transaction_spend_time_unlocked(unp.unlock_time, unlock_height, unlock_timestamp))
						return "INPUT_SPEND_LOCKED_OUT";
					arg.output_keys[i] = unp.public_key;
				}

				total_counter += 1;
				std::unique_lock<std::mutex> lock(mu);
				args.push_back(std::move(arg));
				have_work.notify_all();
			}
			input_index++;
		}
	}
	return std::string();
}

bool RingCheckerMulticore::signatures_valid() const {
	while (true) {
		std::unique_lock<std::mutex> lock(mu);
		if (ready_counter != total_counter) {
			result_ready.wait(lock);
			continue;
		}
		return errors.empty();
	}
}

