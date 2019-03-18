#include "errors.h"

#include <fmt/format.h>

// Error Code implementation.
ErrorCode::ErrorCode() noexcept
    : _code(0)
    , runtime_error("")
{
	_msg = fmt::format("error code [{}]", _code);
}

ErrorCode::ErrorCode(int code, const char* message) noexcept
    : _code(code)
    , runtime_error(message)
{
	_msg = fmt::format("error code [{}]: {}", _code, message);
}

int ErrorCode::code() const noexcept
{
	return _code;
}

ErrorCode::operator bool() const noexcept
{
	return _code != 0;
}

const char* ErrorCode::what() const noexcept
{
	return _msg.c_str();
}

// Log Error implementation.
LogError::LogError(int code, const char* message) noexcept
    : ErrorCode(code, message)
{
}

// HResult Error implementation.
HResultError::HResultError(HRESULT code, const char* message) noexcept
    : ErrorCode(code, message)
{
}
