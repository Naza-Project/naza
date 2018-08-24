// Copyright (c) 2012-2018, The CryptoNote developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

// Copyright (c) 2018-2019, The Naza developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#pragma once

#include <boost/variant.hpp>
#include <functional>
#include <vector>
#include "common/BinaryArray.hpp"
#include "common/Invariant.hpp"
#include "crypto/types.hpp"

#define nazacoin_ALLOW_DEBUG_COMMANDS 1
#define UPGRADE_TO_VERSION_4 1

namespace nazacoin
{
	using crypto::Hash;
	using crypto::PublicKey;
	using crypto::SecretKey;
	using crypto::KeyPair;
	using crypto::KeyDerivation;
	using crypto::KeyImage;
	using crypto::Signature;
	using common::BinaryArray;
	using namespace std::placeholders;  // We enjoy standard bindings
	using Height = uint32_t;
	using Difficulty = uint64_t;
	using Amount = uint64_t;
	using Timestamp = uint32_t;
	using UnlockMoment = uint64_t;
	using SignedAmount = int64_t;

	struct CoinbaseInput
	{
		Height block_index = 0;
	};

	struct KeyInput
	{
		Amount amount = 0;
		std::vector<uint32_t> output_indexes;
		KeyImage key_image;
	};

	struct KeyOutput
	{
		PublicKey key;
	};

	using TransactionInput = boost::variant<CoinbaseInput, KeyInput>;
	using TransactionOutputTarget = boost::variant<KeyOutput>;
	struct TransactionOutput 
	{
		Amount amount = 0;
		TransactionOutputTarget target;
	};

	struct TransactionPrefix
	{
		uint8_t version = 0;
		UnlockMoment unlock_time = 0;
		std::vector<TransactionInput> inputs;
		std::vector<TransactionOutput> outputs;
		BinaryArray extra;
	};

	struct Transaction : public TransactionPrefix
	{
		std::vector<std::vector<Signature>> signatures;
	};

	struct BaseTransaction : public TransactionPrefix {}; 

	struct ParentBlock 
	{
		uint8_t major_version = 0;
		uint8_t minor_version = 0;
		Hash previous_block_hash;
		uint16_t transaction_count = 0;  
		std::vector<Hash> base_transaction_branch;
		BaseTransaction base_transaction;
		std::vector<Hash> blockchain_branch;
	};

	struct BlockHeader 
	{
		uint8_t major_version = 0;
		uint8_t minor_version = 0;
		uint32_t nonce = 0;
		Timestamp timestamp = 0;
		Hash previous_block_hash;
	};

	struct BlockTemplate : public BlockHeader
	{
		ParentBlock parent_block;
		Transaction base_transaction;
		std::vector<Hash> transaction_hashes;
	};

	struct AccountPublicAddress 
	{
		PublicKey spend_public_key;
		PublicKey view_public_key;
	};

	struct SendProof
	{  
		Hash transaction_hash;
		AccountPublicAddress address;
		Amount amount = 0;
		std::string message;
		KeyDerivation derivation;
		Signature signature;
	};

	struct AccountKeys
	{
		AccountPublicAddress address;
		SecretKey spend_secret_key;
		SecretKey view_secret_key;
	};

	struct RawBlock
	{
		BinaryArray block;  
		std::vector<BinaryArray> transactions;
	};

	class Block
	{
	public:
		BlockTemplate header;
		std::vector<Transaction> transactions;

		bool from_raw_block(const RawBlock &);
		bool to_raw_block(RawBlock &) const;
	};

	inline bool operator==(const AccountPublicAddress &a, const AccountPublicAddress &b) 
	{
		return std::tie(a.view_public_key, a.spend_public_key) == std::tie(b.view_public_key, b.spend_public_key);
	}

	inline bool operator!=(const AccountPublicAddress &a, const AccountPublicAddress &b) { return !operator==(a, b); }

	inline bool operator<(const AccountPublicAddress &a, const AccountPublicAddress &b)
	{
		return std::tie(a.view_public_key, a.spend_public_key) < std::tie(b.view_public_key, b.spend_public_key);
	}

}  

namespace seria
{
	class ISeria;

	void ser(nazacoin::Hash &v, ISeria &s);
	void ser(nazacoin::KeyImage &v, ISeria &s);
	void ser(nazacoin::PublicKey &v, ISeria &s);
	void ser(nazacoin::SecretKey &v, ISeria &s);
	void ser(nazacoin::KeyDerivation &v, ISeria &s);
	void ser(nazacoin::Signature &v, ISeria &s);
	void ser_members(nazacoin::AccountPublicAddress &v, ISeria &s);
	void ser_members(nazacoin::SendProof &v, ISeria &s);
	void ser_members(nazacoin::TransactionInput &v, ISeria &s);
	void ser_members(nazacoin::TransactionOutput &v, ISeria &s);
	void ser_members(nazacoin::TransactionOutputTarget &v, ISeria &s);
	void ser_members(nazacoin::CoinbaseInput &v, ISeria &s);
	void ser_members(nazacoin::KeyInput &v, ISeria &s);
	void ser_members(nazacoin::KeyOutput &v, ISeria &s);
	void ser_members(nazacoin::TransactionPrefix &v, ISeria &s);
	void ser_members(nazacoin::BaseTransaction &v, ISeria &s);
	void ser_members(nazacoin::Transaction &v, ISeria &s);
	void ser_members(nazacoin::BlockTemplate &v, ISeria &s);
	void ser_members(nazacoin::BlockHeader &v, ISeria &s);
	void ser_members(nazacoin::ParentBlock &v, ISeria &s);
	void ser_members(nazacoin::RawBlock &v, ISeria &s);
	void ser_members(nazacoin::Block &v, ISeria &s);
}
