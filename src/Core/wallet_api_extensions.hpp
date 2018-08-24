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
			struct GetKeys
			{
				static std::string method() { return "get_keys"; }

				struct Request
				{
					std::string address;
				};

				struct Response
				{
					BinaryArray keys;
				};
			};
		}
	}
}

namespace seria
{
	void ser_members(nazacoin::api::extensions::GetKeys::Request &v, ISeria &s);
	void ser_members(nazacoin::api::extensions::GetKeys::Response &v, ISeria &s);
}