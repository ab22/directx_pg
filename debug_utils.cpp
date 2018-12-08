#include "debug_utils.h"

using logger::LogMode;

void logger::log(logger::LogMode mode, std::string_view msg,
	const char* file, int line)
{
	std::stringstream ss;
	std::tm           time_result;
	auto              now      = std::chrono::system_clock::now();
	auto              time_now = std::chrono::system_clock::to_time_t(now);

	::localtime_s(&time_result, &time_now);

	ss << std::put_time(&time_result, "%Y-%m-%d %X") << ' ';

	switch(mode) {
		case LogMode::debug:
			ss << "[DEBUG]: ";
			break;

		case LogMode::info:
			ss << "[INFO]: ";
			break;

		case LogMode::warn:
			ss << "[WARN]: ";
			break;

		case LogMode::error:
			ss << "[ERROR]: ";
			break;

		case LogMode::fatal:
			ss << "[FATAL]: ";
			break;

		default:
			break;
	}

	ss << msg;

	if (file != nullptr) {
		ss << " on " << file << ":" << line;
	}

	ss << '\n';

	std::cout << ss.str();
}

void logger::log_debug(std::string_view msg, const char* file, int line)
{
	logger::log(LogMode::debug, msg, file, line);
}

void logger::log_info(std::string_view msg, const char* file, int line)
{
	logger::log(LogMode::info, msg, file, line);
}

void logger::log_warn(std::string_view msg, const char* file, int line)
{
	logger::log(LogMode::warn, msg, file, line);
}

void logger::log_error(std::string_view msg, const char* file, int line)
{
	logger::log(LogMode::error, msg, file, line);
}

void logger::log_fatal(std::string_view msg, const char* file, int line)
{
	logger::log(LogMode::fatal, msg, file, line);
}

void logger::log_dx_error(HRESULT hr, std::string_view msg, const char* file, int line)
{
	LPSTR buffer = nullptr;

	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM
		| FORMAT_MESSAGE_ALLOCATE_BUFFER
		| FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPTSTR>(&buffer),
		0,
		nullptr);

	if (buffer != nullptr) {
		std::stringstream dxerr_msg;
		dxerr_msg << msg << ": " << buffer;
		LocalFree(reinterpret_cast<HLOCAL>(buffer));

		logger::log_error(dxerr_msg.str(), file, line);
	} else {
		logger::log_error(msg, file, line);
	}
}
