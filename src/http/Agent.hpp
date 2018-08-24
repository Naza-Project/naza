// Copyright (c) 2012-2018, The CryptoNote developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

// Copyright (c) 2018-2019, The Naza developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#pragma once

#include <chrono>
#include <algorithm>
#include <deque>
#include <memory>
#include <set>
#include "ResponseParser.hpp"
#include "common/MemoryStreams.hpp"
#include "platform/Network.hpp"

namespace http {

class Request;

class Agent {
	class Connection {
	public:
		using handler = std::function<void()>;

		explicit Connection(handler r_handler, handler d_handler);

		bool connect(const std::string &address, uint16_t port);
		bool is_open() const { return sock.is_open(); }
		bool read_next(ResponseData &request);
		void write(RequestData &&response);

		void disconnect();

	private:
		common::CircularBuffer buffer;
		std::deque<common::StringStream> responses;

		response request;
		ResponseParser parser;
		bool receiving_body;
		common::StringStream receiving_body_stream;

		bool waiting_write_response;

		void advance_state(bool called_from_runloop);
		void write();
		void on_disconnect();
		void clear();

		handler r_handler;
		handler d_handler;

		platform::TCPSocket sock;
		bool keep_alive;
	};

	friend class Request;

	Request *sent_request;
	std::string address;
	uint16_t port;
	Connection client;
	platform::Timer reconnect_timer;
	std::chrono::steady_clock::time_point request_start;

	void on_client_response();
	void on_client_disconnect();
	void on_reconnect_timer();

	void set_request(Request *req);
	void cancel_request(Request *req);

public:
	Agent(const std::string &address, uint16_t port);
	~Agent();
};

class Request {
public:

	using R_handler = std::function<void(ResponseData &&resp)>;
	using E_handler = std::function<void(std::string err)>;

	Request(Agent &agent, RequestData &&req, R_handler r_handler, E_handler e_handler);
	~Request();

private:
	friend class Agent;
	Agent &agent;
	RequestData req;
	R_handler r_handler;
	E_handler e_handler;
};
}  // namespace http
