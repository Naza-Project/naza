// Copyright (c) 2012-2018, The CryptoNote developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

// Copyright (c) 2018-2019, The Naza developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#pragma once

#if platform_USE_SQLITE
#include "platform/DBsqlite3.hpp"
namespace platform {
using DB = DBsqlite;
}
#else
#include "platform/DBlmdb.hpp"
namespace platform {
using DB = DBlmdb;
}
#endif
