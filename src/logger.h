#pragma once

#include <Windows.h>
#include <string_view>

namespace logger {
	enum class LogMode : unsigned char
	{
		debug,
		info,
		warn,
		error,
		fatal
	};

	const char* log_mode_text(LogMode);

	void log(LogMode, std::string_view msg);
	void log_debug(std::string_view msg);
	void log_info(std::string_view msg);
	void log_warn(std::string_view msg);
	void log_error(std::string_view);
	void log_fatal(std::string_view msg);

	void log_sys_error(HRESULT, std::string_view msg);
} // namespace logger
