#pragma once

#include <string_view>
#include <Windows.h>

namespace logger {
	enum class LogMode: unsigned char {
		debug,
		info,
		warn,
		error,
		fatal
	};

	const char* log_mode_text(LogMode);

	void log(LogMode, std::string_view msg);
	void log(LogMode, std::string_view msg, const char* file, int line);
	void log_debug(std::string_view msg, const char* file = nullptr, int line = 0);
	void log_info(std::string_view msg, const char* file = nullptr, int line = 0);
	void log_warn(std::string_view msg, const char* file = nullptr, int line = 0);
	void log_error(std::string_view msg, const char* file = nullptr, int line = 0);
	void log_fatal(std::string_view msg, const char* file = nullptr, int line = 0);

	void log_sys_error(HRESULT, std::string_view msg, const char* file = nullptr, int line = 0);
}
