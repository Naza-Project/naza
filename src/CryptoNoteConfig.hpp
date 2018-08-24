// Copyright (c) 2012-2018, The CryptoNote developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

// Copyright (c) 2018-2019, The Naza developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>


namespace nazacoin
{
	namespace parameters
	{

		const uint64_t CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX = 126;

		const uint32_t DIFFICULTY_TARGET = 60;
		const uint64_t DIFFICULTY_WINDOW_V2 = 60;
		const uint64_t DIFFICULTY_BLOCKS_COUNT_V2 = DIFFICULTY_WINDOW_V2 + 1;
		const uint64_t DIFFICULTY_SCATTER_V2 = 32;


		const uint32_t BLOCKCHAIN_TIMESTAMP_CHECK_WINDOW_V2 = 11;
		const uint32_t BLOCKCHAIN_TIMESTAMP_CHECK_WINDOW = 60;

		const uint32_t CRYPTONOTE_BLOCK_FUTURE_TIME_LIMIT_V2 = DIFFICULTY_TARGET * 3;
		const uint32_t CRYPTONOTE_MAX_BLOCK_NUMBER = 500000000;
		const uint32_t CRYPTONOTE_MAX_BLOCK_BLOB_SIZE = 500000000;
		const uint32_t CRYPTONOTE_MAX_TX_SIZE = 1000000000;

		const uint32_t CRYPTONOTE_MINED_MONEY_UNLOCK_WINDOW = 10;
		const uint32_t CRYPTONOTE_BLOCK_FUTURE_TIME_LIMIT = 60 * 60 * 2;
		const size_t CRYPTONOTE_REWARD_BLOCKS_WINDOW = 100;
		const size_t CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE = 100000;
		const size_t CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V2 = 20000;
		const size_t CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V1 = 10000;
		const size_t CRYPTONOTE_COINBASE_BLOB_RESERVED_SIZE = 600;
		const size_t CRYPTONOTE_DISPLAY_DECIMAL_POINT = 10;

		// MONEY_SUPPLY - total number coins to be generated
		const uint64_t MONEY_SUPPLY = std::numeric_limits<uint64_t>::max();
		const unsigned EMISSION_SPEED_FACTOR = 16;
		static_assert(EMISSION_SPEED_FACTOR <= 8 * sizeof(uint64_t), "Bad EMISSION_SPEED_FACTOR");

		const uint64_t MINIMUM_FEE = 1000000;
		const uint64_t DEFAULT_DUST_THRESHOLD = 1000000;

		const uint32_t HARDFORK_V1_HEIGHT = 7070;
		const uint32_t HARDFORK_V2_HEIGHT = 7080;
		const uint32_t HARDFORK_V3_HEIGHT = 4294967294;

		constexpr uint32_t EXPECTED_NUMBER_OF_BLOCKS_PER_DAY(uint32_t difficulty_target) 
		{
			return 24 * 60 * 60 / difficulty_target;
		}
		constexpr uint32_t DIFFICULTY_WINDOW(uint32_t difficulty_target) 
		{
			return EXPECTED_NUMBER_OF_BLOCKS_PER_DAY(difficulty_target);
		}
		const size_t DIFFICULTY_CUT = 60;
		const size_t DIFFICULTY_LAG = 15;
		static_assert(2 * DIFFICULTY_CUT <= DIFFICULTY_WINDOW(DIFFICULTY_TARGET) - 2, "Bad DIFFICULTY_WINDOW or DIFFICULTY_CUT");

		const size_t MAX_BLOCK_SIZE_INITIAL = 20 * 1024;
		const uint64_t MAX_BLOCK_SIZE_GROWTH_SPEED_NUMERATOR = 100 * 1024;
		constexpr uint64_t MAX_BLOCK_SIZE_GROWTH_SPEED_DENOMINATOR(uint32_t difficulty_target)
		{
			return 365 * 24 * 60 * 60 / difficulty_target;
		}

		// After next hardfork remove settings below
		const uint32_t CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_BLOCKS = 1;
		constexpr uint32_t CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_SECONDS(uint32_t difficulty_target) 
		{
			return difficulty_target * CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_BLOCKS;
		}

		const uint32_t CRYPTONOTE_MEMPOOL_TX_LIVETIME = 60 * 60 * 24;

		const uint32_t UPGRADE_HEIGHT_V2 = 1;
		const uint32_t UPGRADE_HEIGHT_V3 = 2;
		const uint32_t UPGRADE_HEIGHT_V4 = HARDFORK_V2_HEIGHT;

		const char CRYPTONOTE_BLOCKS_FILENAME[] = "blocks.bin";
		const char CRYPTONOTE_BLOCKINDEXES_FILENAME[] = "blockindexes.bin";
	}  // parameters

	const char CRYPTONOTE_NAME[] = "naza";

	const uint8_t CURRENT_TRANSACTION_VERSION = 1;

	const size_t BLOCKS_IDS_SYNCHRONIZING_DEFAULT_COUNT = 10000;
	const size_t BLOCKS_SYNCHRONIZING_DEFAULT_COUNT = 100;
	const size_t COMMAND_RPC_GET_BLOCKS_FAST_MAX_COUNT = 1000;

	const int P2P_DEFAULT_PORT = 18640;
	const int RPC_DEFAULT_PORT = 18641;
	const int WALLET_RPC_DEFAULT_PORT = 18642;

	const size_t P2P_LOCAL_WHITE_PEERLIST_LIMIT = 1000;
	const size_t P2P_LOCAL_GRAY_PEERLIST_LIMIT = 5000;

	const size_t P2P_CONNECTION_MAX_WRITE_BUFFER_SIZE = 32 * 1024 * 1024;
	const uint32_t P2P_DEFAULT_CONNECTIONS_COUNT = 8;
	const uint32_t P2P_DEFAULT_WHITELIST_CONNECTIONS_PERCENT = 70;
	const uint32_t P2P_DEFAULT_HANDSHAKE_INTERVAL = 60;
	const uint32_t P2P_DEFAULT_PACKET_MAX_SIZE = 50000000;
	const uint32_t P2P_DEFAULT_PEERS_IN_HANDSHAKE = 250;
	const uint32_t P2P_DEFAULT_CONNECTION_TIMEOUT = 5000;
	const uint32_t P2P_DEFAULT_PING_CONNECTION_TIMEOUT = 2000;
	const uint32_t P2P_DEFAULT_INVOKE_TIMEOUT = 60 * 2 * 1000;
	const uint32_t P2P_DEFAULT_HANDSHAKE_INVOKE_TIMEOUT = 5000;
	const char     P2P_STAT_TRUSTED_PUBLIC_KEY[] = "9c8eab90a47fa4e8ac674abb3c93a5ac66de703cc9247ac36a429aacbe8a321a";

	const char *const SEED_NODES[] =
	{
		"104.168.153.55:18640",
		"104.168.151.101:18640",
		"104.168.144.151:18640",
		"104.168.144.234:18640"
	};

	struct CheckpointData
	{
		uint32_t height;
		const char *hash;
	};

	constexpr const CheckpointData CHECKPOINTS[] =
	{
		{7100, "57319d5fda1e7c573abaa9b75bffbb02ead36b6f9a84fd9557cdfa22de071f16"},
		{15000, "76f1ac86b83c4faedd9fd6b16789e59a477ceb7faa0188aa7a48cd1b7032f905"},
		{20000, "750b7d1699fc8b770133863205bf5460e3278d47e306430596935dd98c793a0e"},
		{30000, "9eeacb09ea68f9d40570dffefba8fe385bccd605e9e1fda45dff937027b3c336"},
		{80000, "10274d477f19178dde88d4f199240f090273b4aadcb5f6f24fbfde3ec4cdc28d" }
	};
}
