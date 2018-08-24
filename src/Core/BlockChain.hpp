// Copyright (c) 2012-2018, The CryptoNote developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

// Copyright (c) 2018-2019, The Naza developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#pragma once

#include <bitset>
#include <deque>
#include <unordered_map>
#include "CryptoNote.hpp"
#include "logging/LoggerMessage.hpp"
#include "Currency.hpp"
#include "platform/DB.hpp"
#include "platform/ExclusiveLock.hpp"
#include "rpc_api.hpp"

namespace crypto 
{
	class CryptoNightContext;
}
namespace nazacoin 
{
	class Config;
	class Currency;

	enum class BroadcastAction { BROADCAST_ALL, NOTHING, BAN };
	enum class AddTransactionResult 
	{
		BAN,
		BROADCAST_ALL,
		ALREADY_IN_POOL,
		INCREASE_FEE,
		FAILED_TO_REDO,
		OUTPUT_ALREADY_SPENT
	};

	struct PreparedBlock
	{
		BinaryArray block_data;
		RawBlock raw_block;
		Block block;
		Hash bid;
		Hash base_transaction_hash;
		size_t coinbase_tx_size = 0;
		size_t parent_block_size = 0;
		Hash long_block_hash;  

		explicit PreparedBlock(BinaryArray &&ba, crypto::CryptoNightContext *context);
		explicit PreparedBlock(RawBlock &&rba, crypto::CryptoNightContext *context);  // we get raw blocks from p2p
		PreparedBlock() = default;
	};

	class BlockChain {
	public:

		using DB = platform::DB;

		explicit BlockChain(logging::ILogger &, const Config &config, const Currency &, bool read_only);
		virtual ~BlockChain() = default;

		const Hash &get_genesis_bid() const { return m_genesis_bid; }
		Hash get_tip_bid() const { return m_tip_bid; }
		Height get_tip_height() const { return m_tip_height; }
		const api::BlockHeader &get_tip() const;
		template<typename T>
		void get_tips(Height, Height, T &) const;
		template<typename T>
		void get_txs(const std::vector<Hash> &, T &) const;

		std::vector<api::BlockHeader> get_tip_segment(
			const api::BlockHeader &prev_info, Height window, bool add_genesis) const;

		bool read_chain(Height height, Hash *bid) const;
		bool in_chain(Height height, Hash bid) const;
		bool read_block(const Hash &bid, RawBlock *rb) const;
		bool read_block(const Hash &bid, BinaryArray *block_data, RawBlock *rb) const; // rb can be null here
		bool has_block(const Hash &bid) const;
		bool read_header(const Hash &bid, api::BlockHeader *info, Height hint = 0) const;
		bool read_transaction(const Hash &tid, Transaction *tx, Height *block_height, Hash *block_hash, size_t *index_in_block, uint32_t *binary_size) const;


		BroadcastAction add_block(const PreparedBlock &pb, api::BlockHeader *info, const std::string &source_address);

		std::vector<Hash> get_sparse_chain() const;
		std::vector<api::BlockHeader> get_sync_headers(const std::vector<Hash> &sparse_chain, size_t max_count) const;
		std::vector<Hash> get_sync_headers_chain(
			const std::vector<Hash> &sparse_chain, Height *start_height, size_t max_count) const;

		Height find_blockchain_supplement(const std::vector<Hash> &remote_block_ids) const;
		Height get_timestamp_lower_bound_block_index(Timestamp) const;

		void test_undo_everything(Height new_tip_height);
		void test_print_structure(Height n_confirmations) const;

		void test_prune_oldest();

		void db_commit();

		bool internal_import(); 
		Height internal_import_known_height() const { return static_cast<Height>(m_internal_import_chain.size()); }

		Height largest_window() const; 


	protected:
		Difficulty get_tip_cumulative_difficulty() const { return m_tip_cumulative_difficulty; }
		bool read_next_internal_block(Hash *bid) const;

		std::vector<Hash> m_internal_import_chain;
		void start_internal_import();

		virtual std::string check_standalone_consensus(
			const PreparedBlock &pb, api::BlockHeader *info, const api::BlockHeader &prev_info, bool check_pow) const = 0;
		virtual bool redo_block(const Hash &bhash, const Block &block, const api::BlockHeader &info) = 0;
		virtual void undo_block(const Hash &bhash, const Block &block, Height height) = 0;
		bool redo_block(const Hash &bhash, const BinaryArray &block_data, const RawBlock &raw_block, const Block &block, const api::BlockHeader &info, const Hash &base_transaction_hash);
		void debug_check_transaction_invariants(const RawBlock &raw_block, const Block &block, const api::BlockHeader &info,
			const Hash &base_transaction_hash) const;
		void undo_block(const Hash &bhash, const RawBlock &raw_block, const Block &block, Height height);
		virtual void tip_changed() {} 
		virtual void on_reorganization(
			const std::map<Hash, std::pair<Transaction, BinaryArray>> &undone_transactions, bool undone_blocks) = 0;

		const Hash m_genesis_bid;
		const std::string m_coin_folder;
		Hash get_common_block(const Hash &bid1, const Hash &bid2, std::vector<Hash> *chain1, std::vector<Hash> *chain2) const; // both can be null

		DB m_db;
		logging::LoggerRef m_log;
		const Config &m_config;
		const Currency &m_currency;

		static const std::string version_current;

	private:
		Hash m_tip_bid;
		Difficulty m_tip_cumulative_difficulty = 0;
		Height m_tip_height = -1;
		void read_tip();
		void push_chain(const api::BlockHeader &header);
		void pop_chain(const Hash &new_tip_bid);
		Hash read_chain(Height height) const;

		mutable std::unordered_map<Hash, api::BlockHeader> header_cache;
		std::deque<api::BlockHeader> m_header_tip_window;
		api::BlockHeader read_header(const Hash &bid, Height hint = 0) const;

		void store_block(const Hash &bid, const BinaryArray &block_data);

		void store_header(const Hash &bid, const api::BlockHeader &header);

		bool reorganize_blocks(
			const Hash &switch_to_chain, const PreparedBlock &recent_pb, const api::BlockHeader &recent_info);

		void check_children_counter(Difficulty cd, const Hash &bid, int value);
		void modify_children_counter(Difficulty cd, const Hash &bid, int delta);
		bool get_oldest_tip(Difficulty *cd, Hash *bid) const;
		bool prune_branch(Difficulty cd, Hash bid);

		void for_each_tip(std::function<bool(Difficulty cd, Hash bid)> fun) const;
















	};

}  // namespace nazacoin
