// Copyright (c) 2012-2018, The CryptoNote developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

// Copyright (c) 2018-2019, The Naza developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#pragma once

#include <list>
#include <memory>
#include <mutex>
#include "LoggerGroup.hpp"
#include "LoggerMessage.hpp"
#include "common/JsonValue.hpp"

namespace logging {

	class LoggerManager : public LoggerGroup {
	public:
		LoggerManager() =default;
		void configure_default(const std::string &log_folder, const std::string &log_prefix);  // log_folder must exist
		void configure(const common::JsonValue &val);                                          // from json config
		virtual void write(
			const std::string &category, Level level, boost::posix_time::ptime time, const std::string &body) override;

	private:
		std::vector<std::unique_ptr<CommonLogger>> loggers;
		std::mutex reconfigure_lock;
	};
}
