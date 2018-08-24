// Copyright (c) 2012-2018, The CryptoNote developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

// Copyright (c) 2018-2019, The Naza developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.


#pragma once

#include <cstdint>
#include <string>
#include "common/CommandLine.hpp"

namespace nazacoin {

	struct MiningConfig {
		explicit MiningConfig(common::CommandLine &cmd);

		std::string mining_address;
		std::string nazad_ip= std::move("127.0.0.1");
		uint16_t nazad_port = RPC_DEFAULT_PORT;
		size_t thread_count = std::thread::hardware_concurrency();

		size_t blocks_limit = 0;  
	};

}  // namespace nazacoin
