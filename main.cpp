#include "d3dapp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
	// Enable run-time memory check for debug builds
	#if defined(DEBUG) | defined(_DEBUG)
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	#endif

	D3DApp d3dapp(hInstance);

	if (!d3dapp.init()) {
		MessageBox(NULL, TEXT("D3DApp init failed!"), 0, MB_ICONERROR);
		return -1;
	}

	return d3dapp.run();
}
