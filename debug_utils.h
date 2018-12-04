#pragma once

#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <string>
#include <string_view>
#include <cstddef>
#include <Windows.h>

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

namespace logger {
	enum class LogMode: unsigned char {
		debug,
		info,
		warn,
		error,
		fatal
	};

	void log(LogMode, std::string_view msg, const char* file = nullptr, int line = 0);
	void log_debug(std::string_view msg, const char* file = nullptr, int line = 0);
	void log_info(std::string_view msg, const char* file = nullptr, int line = 0);
	void log_warn(std::string_view msg, const char* file = nullptr, int line = 0);
	void log_error(std::string_view msg, const char* file = nullptr, int line = 0);
	void log_fatal(std::string_view msg, const char* file = nullptr, int line = 0);

	void log_dx_error(HRESULT, std::string_view msg, const char* file = nullptr, int line = 0);
}
