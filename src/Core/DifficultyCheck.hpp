// Copyright (c) 2012-2018, The CryptoNote developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

// Copyright (c) 2018-2019, The Naza developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#pragma once

#include "CryptoNote.hpp"

namespace nazacoin {
struct DifficultyCheck {
	Height height;
	const char *hash;
	Difficulty cumulative_difficulty;
};

extern const DifficultyCheck *difficulty_check;
extern const size_t difficulty_check_count;
}
