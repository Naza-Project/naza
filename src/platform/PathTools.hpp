// Copyright (c) 2012-2018, The CryptoNote developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

// Copyright (c) 2018-2019, The Naza developers.
// Licensed under the GNU Lesser General Public License. See LICENSE for details.

#pragma once

#include <string>
#include <vector>
#include "common/BinaryArray.hpp"

// For documentation
#if defined(__MACH__)
#define platform_DEFAULT_DATA_FOLDER_PATH_PREFIX "~/Library/Application Support/"
#elif defined(_WIN32)
#define platform_DEFAULT_DATA_FOLDER_PATH_PREFIX "%appdata%/"
#else  // defined(__linux__) and unknown platforms
#define platform_DEFAULT_DATA_FOLDER_PATH_PREFIX "~/."
#endif

namespace platform {
std::string get_default_data_directory(
    const std::string &cryptonote_name);  // we avoid including app-specific headers into our platform code

std::string get_app_data_folder(const std::string &app_name);

std::string get_os_version_string();
std::string get_platform_name();
bool folder_exists(const std::string &path);
bool create_folder_if_necessary(const std::string &path);   // Only last element
bool create_folders_if_necessary(const std::string &path);  // Recursively all elements
bool atomic_replace_file(const std::string &from_path, const std::string &to_path);
bool copy_file(const std::string &from_path, const std::string &to_path);
bool remove_file(const std::string &path);
std::vector<std::string> get_filenames_in_folder(const std::string &path);
std::string get_filename_without_folder(const std::string &path);
// std::string strip_trailing_slashes(const std::string & path);
bool load_file(const std::string &filepath, std::string &buf);
bool load_file(const std::string &filepath, common::BinaryArray &buf);
bool save_file(const std::string &filepath, const void *buf, size_t size);
bool atomic_save_file(const std::string &filepath, const void *buf, size_t size, const std::string &tmp_filepath);
inline bool save_file(const std::string &filepath, const std::string &buf) {
	return save_file(filepath, buf.data(), buf.size());
}
}
