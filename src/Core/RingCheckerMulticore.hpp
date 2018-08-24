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


struct RingSignatureArg {
	Hash tx_prefix_hash;
	KeyImage key_image;
	std::vector<PublicKey> output_keys;
	std::vector<Signature> signatures;
};

class RingCheckerMulticore {
	std::vector<std::thread> threads;
	mutable std::mutex mu;
	mutable std::condition_variable have_work;
	mutable std::condition_variable result_ready;
	bool quit = false;

	size_t total_counter = 0;
	size_t ready_counter = 0;
	std::vector<std::string> errors;

	std::deque<RingSignatureArg> args;
	int work_counter = 0;
	void thread_run();

public:
	RingCheckerMulticore();
	~RingCheckerMulticore();
	void cancel_work();
	std::string start_work_get_error(IBlockChainState *state, const Currency &currency, const Block &block,
	    Height unlock_height, Timestamp unlock_timestamp);  
	bool signatures_valid() const;
};


}
