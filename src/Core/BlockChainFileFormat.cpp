// Copyright (c) 2012-2018, The CryptoNote developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

// Copyright (c) 2018-2019, The Naza developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#include "BlockChainFileFormat.hpp"

#include "BlockChainState.hpp"
#include "platform/PathTools.hpp"
#include "seria/BinaryInputStream.hpp"
#include "seria/BinaryOutputStream.hpp"

using namespace common;
using namespace nazacoin;



LegacyBlockChainReader::LegacyBlockChainReader(const std::string &index_file_name, const std::string &item_file_name) {
	try {
		m_indexes_file = std::make_unique<platform::FileStream>(index_file_name, platform::FileStream::READ_EXISTING);
		m_items_file   = std::make_unique<platform::FileStream>(item_file_name, platform::FileStream::READ_EXISTING);

		m_indexes_file->seek(0, SEEK_END);
		uint64_t m_indexesFileSize = m_indexes_file->tellp();
		m_indexes_file->seek(0, SEEK_SET);
		uint64_t max_hei  = (m_indexesFileSize - sizeof(uint64_t)) / sizeof(uint32_t);
		uint64_t read_hei = 0;
		m_indexes_file->read(reinterpret_cast<char *>(&read_hei), sizeof(uint64_t));
		m_count = boost::lexical_cast<Height>(std::min(read_hei, max_hei));
	} catch (const std::runtime_error &) {
	}
}

LegacyBlockChainReader::~LegacyBlockChainReader() {
	{
		std::unique_lock<std::mutex> lock(mu);
		quit = true;
		have_work.notify_all();
	}
	if (th.joinable())
		th.join();
}

void LegacyBlockChainReader::load_offsets() {
	if (m_count == 0 || !m_offsets.empty())
		return;
	uint64_t pos = 0;
	try {
		m_items_file->seek(0, SEEK_END);
		uint64_t m_itemsFileSize = m_items_file->tellp();
		std::vector<uint32_t> item_sizes(m_count);
		m_indexes_file->read(reinterpret_cast<char *>(item_sizes.data()), m_count * sizeof(uint32_t));
		for (size_t i = 0; i < item_sizes.size(); ++i) {
			m_offsets.emplace_back(pos);
			pos += item_sizes[i];
			if (pos > m_itemsFileSize) 
				return;
		}
	} catch (const std::runtime_error &) {
	}
	m_offsets.emplace_back(pos);
}

BinaryArray LegacyBlockChainReader::get_block_data_by_index(Height i) {
	load_offsets();
	size_t si = boost::lexical_cast<size_t>(m_offsets.at(i + 1) - m_offsets.at(i));
	m_items_file->seek(m_offsets.at(i), SEEK_SET);
	BinaryArray data_cache(si);
	m_items_file->read(reinterpret_cast<char *>(data_cache.data()), si);
	return data_cache;
}

const size_t MAX_PRELOAD_BLOCKS     = 100;
const size_t MAX_PRELOAD_TOTAL_SIZE = 50 * 1024 * 1024;

void LegacyBlockChainReader::thread_run() {
	while (true) {
		Height to_load = 0;
		{
			std::unique_lock<std::mutex> lock(mu);
			if (quit)
				return;
			if (next_load_height == 0)
				next_load_height = last_load_height;
			if (next_load_height > last_load_height + MAX_PRELOAD_BLOCKS ||
			    total_prepared_data_size > MAX_PRELOAD_TOTAL_SIZE) {
				have_work.wait(lock);
				continue;
			}
			to_load = next_load_height++;
		}
		BinaryArray rba = get_block_data_by_index(to_load);
		PreparedBlock pb(std::move(rba), nullptr);
		{
			std::unique_lock<std::mutex> lock(mu);
			total_prepared_data_size += pb.block_data.size();
			prepared_blocks[to_load] = std::move(pb);
			prepared_blocks_ready.notify_all();
		}
	}
}

static size_t max_ps = 0;
PreparedBlock LegacyBlockChainReader::get_prepared_block_by_index(Height i) {
	load_offsets();
	{
		std::unique_lock<std::mutex> lock(mu);
		if (!th.joinable())
			th           = std::thread(&LegacyBlockChainReader::thread_run, this);
		last_load_height = i;
		have_work.notify_all();
	}
	while (true) {
		std::unique_lock<std::mutex> lock(mu);
		auto pit = prepared_blocks.find(i);
		if (pit == prepared_blocks.end()) {
			prepared_blocks_ready.wait(lock);
			continue;
		}
		PreparedBlock result = std::move(pit->second);
		pit                  = prepared_blocks.erase(pit);
		max_ps               = std::max(max_ps, total_prepared_data_size);
		total_prepared_data_size -= result.block_data.size();
		return result;
	}
}

bool LegacyBlockChainReader::import_blocks(BlockChainState *block_chain) {
	try {
		auto idea_start = std::chrono::high_resolution_clock::now();
		while (block_chain->get_tip_height() + 1 < get_block_count()) {
			BinaryArray rba = get_block_data_by_index(block_chain->get_tip_height() + 1);
			PreparedBlock pb(std::move(rba), nullptr);
			api::BlockHeader info;
			if (block_chain->add_block(pb, &info, "blocks_file") != BroadcastAction::BROADCAST_ALL) {
				std::cout << "block_chain.add_block !BROADCAST_ALL block=" << block_chain->get_tip_height() + 1
				          << std::endl;
				block_chain->db_commit();
				return false;
			}
			auto idea_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
			    std::chrono::high_resolution_clock::now() - idea_start);
			if (idea_ms.count() > 200)  // import in chunks of 0.2 seconds
				break;
		}
	} catch (const std::exception &ex) {
		std::cout << "Exception while importing blockchain file, what=" << ex.what() << std::endl;
		return false;
	} catch (...) {
		std::cout << "Unknown exception while importing blockchain file" << std::endl;
		return false;
	}
	return block_chain->get_tip_height() + 1 < get_block_count();  // Not finished
}

bool LegacyBlockChainReader::import_blockchain2(const std::string &coin_folder,
    BlockChainState *block_chain,
    Height max_height) {


	LegacyBlockChainReader reader(coin_folder + "/blockindexes.bin", coin_folder + "/blocks.bin");
	const size_t import_height = std::min(max_height, reader.get_block_count() + 1);
	if (block_chain->get_tip_height() > import_height) {

		return true;
	}
	std::cout << "Importing blocks up to height " << import_height << std::endl;
	auto idea_start  = std::chrono::high_resolution_clock::now();
	auto start_block = block_chain->get_tip_height();

	while (block_chain->get_tip_height() < import_height) {
		PreparedBlock pb = reader.get_prepared_block_by_index(block_chain->get_tip_height() + 1);
		api::BlockHeader info;
		if (block_chain->add_block(pb, &info, "blocks_file") != BroadcastAction::BROADCAST_ALL) {
			std::cout << "block_chain.add_block !BROADCAST_ALL block=" << block_chain->get_tip_height() + 1
			          << std::endl;
			block_chain->db_commit();
			return false;
		}

	}
	block_chain->db_commit();
	auto idea_ms =
	    std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - idea_start);
	std::cout << "Import blocks " << start_block << ":" << block_chain->get_tip_height()
	          << " seconds=" << double(idea_ms.count()) / 1000 << std::endl;
	return true;
}

LegacyBlockChainWriter::LegacyBlockChainWriter(const std::string &index_file_name,
    const std::string &item_file_name,
    uint64_t count)
    : m_items_file(item_file_name, platform::FileStream::TRUNCATE_READ_WRITE)
    , m_indexes_file(index_file_name, platform::FileStream::TRUNCATE_READ_WRITE) {
	m_indexes_file.write(&count, sizeof(count));
}

void LegacyBlockChainWriter::write_block(const nazacoin::RawBlock &raw_block) {
	nazacoin::BinaryArray ba = seria::to_binary(raw_block);
	m_items_file.write(ba.data(), ba.size());
	uint32_t si = static_cast<uint32_t>(ba.size());
	m_indexes_file.write(&si, sizeof si);
}

bool LegacyBlockChainWriter::export_blockchain2(const std::string &export_folder, const BlockChainState &block_chain) {
	auto idea_start = std::chrono::high_resolution_clock::now();
	std::cout << "Start exporting blocks" << std::endl;
	LegacyBlockChainWriter writer(
	    export_folder + "/blockindexes.bin", export_folder + "/blocks.bin", block_chain.get_tip_height() + 1);
	for (Height ha = 0; ha != block_chain.get_tip_height() + 1; ++ha) {
		Hash bid{};
		BinaryArray block_data;
		RawBlock raw_block;
		if (!block_chain.read_chain(ha, &bid) || !block_chain.read_block(bid, &block_data, &raw_block))
			throw std::runtime_error("block_chain.read_block failed");
		writer.write_block(raw_block);
		if (ha % 10000 == 0)
			std::cout << "Exporting block " << ha << "/" << block_chain.get_tip_height() << std::endl;

	}
	auto idea_ms =
	    std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - idea_start);
	std::cout << "Last exported block " << block_chain.get_tip_height() << " seconds=" << double(idea_ms.count()) / 1000
	          << std::endl;
	return true;
}
