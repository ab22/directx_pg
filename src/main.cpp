#include "demo_app.h"
#include "errors.h"
#include <fmt/format.h>

using debug_utils::debug_mode;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, int)
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
	} catch (LogError &e) {
		// Do not call logger::log. We assume the logger will dump the
		// error if possible.
		MessageBoxA(0, e.what(), "Fatal error!", MB_OK | MB_ICONERROR);
		return 0;
	} catch (std::exception& ex) {
		auto msg = fmt::format("Unhandled exception caught: {}", ex.what());

		MessageBoxA(0, msg.c_str(), "Fatal error!", MB_OK | MB_ICONERROR);
		logger::log_fatal(msg);
		return -1;
	}
}
