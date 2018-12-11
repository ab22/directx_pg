#pragma once

#include <exception>
#include <string>

class ErrorCode: public std::runtime_error {
protected:
	int         _code;
	std::string _msg;

public:
	ErrorCode() noexcept;
	ErrorCode(int, const char* message) noexcept;
	ErrorCode(ErrorCode&&) = default;

	virtual int         code() const noexcept;
	virtual const char* what() const noexcept;

	virtual ErrorCode& operator=(const ErrorCode&) = default;
	virtual ErrorCode& operator=(ErrorCode&&)      = default;

	virtual explicit operator bool() const noexcept;
};

class LogError: public ErrorCode {
public:
	LogError(int, const char* message) noexcept;
};
