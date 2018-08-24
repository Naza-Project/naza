// Copyright (c) 2012-2018, The CryptoNote developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

// Copyright (c) 2018-2019, The Naza developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#include "LoggerMessage.hpp"

namespace logging {

	LoggerMessage::LoggerMessage(ILogger &logger, const std::string &category, Level level)
		: std::ostream(this)
		, logger(logger)
		, category(category)
		, log_level(level)
		, timestamp(boost::posix_time::microsec_clock::local_time()) {

	}

	LoggerMessage::~LoggerMessage() {
		if (!str().empty())
			(*this) << std::endl;
	}

	LoggerMessage::LoggerMessage(LoggerMessage &&other)
		: std::ostream(this)
		, logger(other.logger)
		, category(other.category)
		, log_level(other.log_level)
		, timestamp(boost::posix_time::microsec_clock::local_time()) {
		(*this) << other.str();
		other.str(std::string());
	}

	int LoggerMessage::sync() {
		logger.write(category, log_level, timestamp, str());
		str(std::string());
		return 0;
	}
}
