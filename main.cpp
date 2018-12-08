#include "demo_app.h"

using debug_utils::debug_mode;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
	// Enable run-time memory check for debug builds
	if constexpr (debug_mode) {
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	}

	try {
		DemoApp app(hInstance);

		if (!app.init()) {
			MessageBox(NULL, TEXT("App initialization failed!"), 0, MB_ICONERROR);
			return -1;
		}

		return app.run();
	} catch (std::exception& ex) {
		std::stringstream msg;
		msg << "Unhandled exception caught: " << ex.what();
		logger::log_fatal(msg.str());

		throw;
	}

	return -1;
}
