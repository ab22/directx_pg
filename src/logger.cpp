#include "errors.h"
#include "logger.h"
#include "win_utils.h"

#include <fmt/format.h>
#include <fmt/time.h>
#include <iostream>
#include <windowsx.h>

#include <string>

using logger::LogMode;

const char* logger::log_mode_text(LogMode mode)
{
	switch (mode) {
	case LogMode::debug:
		return "DEBUG";

	case LogMode::info:
		return "INFO";

	case LogMode::warn:
		return "WARN";

	case LogMode::error:
		return "ERROR";

	case LogMode::fatal:
		return "FATAL";

	default:
		return "UNDEFINED";
	}
}

void logger::log(LogMode mode, std::string_view msg)
{
	const char* mode_str = log_mode_text(mode);
	std::time_t t        = std::time(nullptr);
	std::tm     tm       = {};
	errno_t     err      = ::localtime_s(&tm, &t);

	if (err != 0) {
		// Attempt to log the error and throw an exception.
		fmt::print("[{}]: {}", mode_str, msg);
		throw LogError(err, "Failed to acquire localtime");
	}

	fmt::print("{:%Y-%m-%d %X} [{}]: {}", tm, mode_str, msg);
}

void logger::log_debug(std::string_view msg)
{
	logger::log(LogMode::debug, msg);
}

void logger::log_info(std::string_view msg)
{
	logger::log(LogMode::info, msg);
}

void logger::log_warn(std::string_view msg)
{
	logger::log(LogMode::warn, msg);
}

void logger::log_error(std::string_view msg)
{
	logger::log(LogMode::error, msg);
}

void logger::log_fatal(std::string_view msg)
{
	logger::log(LogMode::fatal, msg);
}

void logger::log_sys_error(HRESULT hr, std::string_view msg)
{
	using win_utils::LocalAllocDeleter;
	using LPSTR_guard = std::unique_ptr<CHAR, LocalAllocDeleter>;

	LPSTR sys_msg = nullptr;
	auto  msg_len =
	    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER |
	                      FORMAT_MESSAGE_IGNORE_INSERTS,
	                  nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	                  reinterpret_cast<LPTSTR>(&sys_msg), 0, nullptr);

	if (msg_len == 0) {
		auto formatted_msg = fmt::format("{}: [HRESULT:{}]", msg, hr);
		// Log the user message only.
		logger::log_error(formatted_msg);
		throw LogError(GetLastError(), "FormatMessage failed!");
	}

	LPSTR_guard str_guard(sys_msg);
	auto        formatted_msg = fmt::format("{}: [{}] {}", msg, hr, sys_msg);
	logger::log_error(formatted_msg);
}
