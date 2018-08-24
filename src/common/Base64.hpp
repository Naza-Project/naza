// Copyright (c) 2012-2018, The CryptoNote developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

// Copyright (c) 2018-2019, The Naza developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#pragma once

#include <cstdint>
#include <string>
#include "BinaryArray.hpp"

namespace common
{
	namespace base64 
	{
		std::string encode(const BinaryArray &data);
		bool decode(const std::string &enc, BinaryArray *data);
	}
}
