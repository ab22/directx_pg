#include "d3dapp.h"

using debug_utils::debug_mode;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
	// Enable run-time memory check for debug builds
	if constexpr (debug_mode) {
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	}

	try {
		D3DApp d3dapp(hInstance);

		if (!d3dapp.init()) {
			MessageBox(NULL, TEXT("D3DApp init failed!"), 0, MB_ICONERROR);
			return -1;
		}

		return d3dapp.run();
	} catch (std::exception& ex) {
		std::stringstream msg;
		msg << "Unhandled exception caught: " << ex.what();
		logger::log_fatal(msg.str());

		throw;
	}

	return -1;
}
