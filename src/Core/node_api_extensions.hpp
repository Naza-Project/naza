// Copyright (c) 2012-2018, The CryptoNote developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

// Copyright (c) 2018-2019, The Naza developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#pragma once

#include "rpc_api.hpp"

using namespace crypto;

namespace nazacoin
{
	namespace api
	{
		namespace extensions
		{
			struct BlockPreview
			{
				crypto::Hash hash;
				uint32_t height;
				uint32_t timestamp;
				uint32_t size;
				uint32_t tx_count;
				uint64_t difficulty;
			};

			struct TransactionPreview
			{
				Hash hash;
				Amount fee;
				Amount amount_in;
				Amount amount_out;
				size_t size;
				size_t mixin;
				Hash payment_id;
			};

			struct Block
			{
				nazacoin::api::BlockHeader header;
				std::vector<TransactionPreview> transactions;
			};

			struct GetBlocks
			{
				static std::string method() { return "get_blocks_json"; }

				struct Request
				{
					Height height;
				};

				struct Response
				{
					std::vector<BlockPreview> blocks;
				};
			};

			struct GetBlock
			{
				static std::string method() { return "get_block_json"; }

				struct Request
				{
					crypto::Hash hash;
		            Height height;
				};

				struct Response
				{
					Block block;
				};
			};

			struct GetTransaction
			{
				static std::string method() { return "get_transaction_json"; }

				struct Request
				{
					crypto::Hash hash;
				};

				struct Response
				{
					BlockPreview block;
					nazacoin::Transaction transaction;
					TransactionPreview transaction_details;
				};
			};

			struct GetMempool
			{
				static std::string method() { return "get_mempool_json"; }
				using Request = EmptyStruct;

				struct Response
				{
					std::vector<TransactionPreview> transactions;
				};
			};
		}
	}
}

namespace seria
{
	void ser_members(nazacoin::api::extensions::BlockPreview &v, ISeria &s);
	void ser_members(nazacoin::api::extensions::Block &v, ISeria &s);
	void ser_members(nazacoin::api::extensions::GetBlocks::Request &v, ISeria &s);
	void ser_members(nazacoin::api::extensions::GetBlocks::Response &v, ISeria &s);
	void ser_members(nazacoin::api::extensions::GetBlock::Request &v, ISeria &s);
	void ser_members(nazacoin::api::extensions::GetBlock::Response &v, ISeria &s);
	void ser_members(nazacoin::api::extensions::GetTransaction::Request &v, ISeria &s);
	void ser_members(nazacoin::api::extensions::GetTransaction::Response &v, ISeria &s);
	void ser_members(nazacoin::api::extensions::GetMempool::Response &v, ISeria &s);
}
