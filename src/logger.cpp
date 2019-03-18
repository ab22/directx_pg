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

	fmt::print("{:%Y-%m-%d %X} [{}]: {}\n", tm, mode_str, msg);
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
