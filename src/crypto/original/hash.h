// Copyright (c) 2012-2013 The Cryptonote developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <stddef.h>


#include "crypto/hash-ops.h"

namespace crypto {

  extern "C" {

void cn_slow_hash_original(const void *data, size_t length, char *hash);
  }



