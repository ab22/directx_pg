#pragma once

#include "debug_utils.h"

#include <comdef.h>

std::string debug_utils::get_system_error(HRESULT hr)
{
	_com_error err(hr);

	return fmt::format("{}", err.ErrorMessage());
}
