#pragma once

#include "logger.h"

namespace debug_utils {
	#if defined(DEBUG) | defined(_DEBUG)
		constexpr bool debug_mode = true;

		#ifndef LOG_DEBUG
			#define LOG_DEBUG(msg) logger::log_debug(msg, __FILE__, __LINE__)
		#endif

	#else
		constexpr bool debug_mode = false;

		#ifndef LOG_DEBUG
			#define LOG_DEBUG(msg) logger::log_debug(msg)
		#endif
	#endif
}
