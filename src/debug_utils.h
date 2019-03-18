#pragma once

#include "logger.h"
#include "win_utils.h"
#include <fmt/format.h>
#include <string>

namespace debug_utils {
// clang-format off
	#if defined(DEBUG) | defined(_DEBUG)
		constexpr bool debug_mode = true;

		#ifndef DBG_LOG
			#define DBG_LOG(msg) logger::log_debug(fmt::format("{} on {}:{}", msg, __FILE__, __LINE__))
		#endif

	#else
		constexpr bool debug_mode = false;

		#ifndef DBG_LOG
			#define DBG_LOG(msg) logger::log_debug(msg)
		#endif
	#endif

	#if defined(WIN32_LEAN_AND_MEAN)
		constexpr bool win32_lean_and_mean = true;
	#else
		constexpr bool win32_lean_and_mean = false;
	#endif
	// clang-format on

	std::string get_system_error(HRESULT hr);
} // namespace debug_utils
