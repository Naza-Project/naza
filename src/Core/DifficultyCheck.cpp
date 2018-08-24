// Copyright (c) 2012-2018, The CryptoNote developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

// Copyright (c) 2018-2019, The Naza developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#include "DifficultyCheck.hpp"

static const nazacoin::DifficultyCheck table[1] = {
    {UINT32_MAX, "", 0},
    };

const nazacoin::DifficultyCheck *nazacoin::difficulty_check = table;
const size_t nazacoin::difficulty_check_count               = sizeof(table) / sizeof(*table);
