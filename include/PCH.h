#pragma once

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

#include <unordered_set>
#include <chrono>
#include <memory>
#include <Windows.h>
#include <filesystem>
#include <string>


using namespace std::literals;

#if _DEBUG
#define DEBUG_LOG(logger, msg, ...) logger->info(msg, __VA_ARGS__)
#else
#define DEBUG_LOG(logger, msg, ...)
#endif

#include <spdlog/sinks/basic_file_sink.h>
#include "Plugin.h"