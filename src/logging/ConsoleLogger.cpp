// Copyright (c) 2012-2018, The CryptoNote developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

// Copyright (c) 2018-2019, The Naza developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#include "ConsoleLogger.hpp"
#include <iostream>
#include <unordered_map>
#include "common/ConsoleTools.hpp"

namespace logging {

using common::console::Color;

std::mutex ConsoleLogger::mutex;

ConsoleLogger::ConsoleLogger(Level level) : CommonLogger(level) {}

void ConsoleLogger::do_log_string(const std::string &message) {
	std::lock_guard<std::mutex> lock(mutex);
	auto changed_color = false;

	for (auto char_pos = 0; char_pos < message.size(); ++char_pos) {
		if (char_pos + 1 < message.size() && message[char_pos] == ILogger::COLOR_PREFIX) {
			char_pos += 1;
			auto color = static_cast<Color>(message[char_pos] - ILogger::COLOR_LETTER_DEFAULT);
			std::cout << std::flush;
			common::console::set_text_color(color);
			changed_color = true;
		} else {
			std::cout << message[char_pos];
		}
	}

	std::cout << std::flush;
	if (changed_color) {
		common::console::set_text_color(Color::Default);
	}
}
}
