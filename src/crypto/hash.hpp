// Copyright (c) 2012-2018, The CryptoNote developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

// Copyright (c) 2018-2019, The Naza developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#pragma once

#include <stddef.h>

#include "hash-ops.h"
#include "types.hpp"

namespace crypto {


inline Hash cn_fast_hash(const void *data, size_t length) {
	Hash h;
	cn_fast_hash(data, length, h.data);
	return h;
}


class CryptoNightContext {
public:
	CryptoNightContext();
	~CryptoNightContext();

	CryptoNightContext(const CryptoNightContext &) = delete;
	void operator=(const CryptoNightContext &) = delete;

	inline void cn_slow_hash(const void *src_data, size_t length, unsigned char *hash) {
		crypto::cn_slow_hash(data, src_data, length, hash, 0, 0);
	}
	inline Hash cn_slow_hash(const void *src_data, size_t length) {
		Hash hash;
		crypto::cn_slow_hash(data, src_data, length, hash.data, 0, 0);
		return hash;
	}

	inline void cn_lite_slow_hash_v1(const void *src_data, size_t length, unsigned char *hash) {
		crypto::cn_slow_hash(data, src_data, length, hash, 1, 1);
	}
	inline Hash cn_lite_slow_hash_v1(const void *src_data, size_t length) {
		Hash hash;
		crypto::cn_slow_hash(data, src_data, length, hash.data, 1, 1);
		return hash;
	}

private:
	void *data;
};

inline Hash tree_hash(const Hash *hashes, size_t count) {
	Hash root_hash;
	tree_hash(reinterpret_cast<const unsigned char(*)[HASH_SIZE]>(hashes), count, root_hash.data);
	return root_hash;
}

inline void tree_branch(const Hash *hashes, size_t count, Hash *branch) {
	tree_branch(reinterpret_cast<const unsigned char(*)[HASH_SIZE]>(hashes), count,
	    reinterpret_cast<unsigned char(*)[HASH_SIZE]>(branch));
}

inline Hash tree_hash_from_branch(const Hash *branch, size_t depth, const Hash &leaf, const void *path) {
	Hash root_hash;
	tree_hash_from_branch(
	    reinterpret_cast<const unsigned char(*)[HASH_SIZE]>(branch), depth, leaf.data, path, root_hash.data);
	return root_hash;
}
}
