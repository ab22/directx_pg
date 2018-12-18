#pragma once

#include <Windows.h>
#include <cassert>

namespace win_utils {
	struct LocalAllocDeleter {
		void operator()(HLOCAL mem)
		{
			auto res = LocalFree(mem);
			assert(res == 0);
		}
	};
} // namespace win_utils
