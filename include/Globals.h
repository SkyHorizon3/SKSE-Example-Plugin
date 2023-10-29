#pragma once

#include <unordered_set>
#include <string>
#include <memory>
#include <spdlog/sinks/basic_file_sink.h>

inline std::shared_ptr<spdlog::logger> g_Logger;

inline bool isLoaded = false;