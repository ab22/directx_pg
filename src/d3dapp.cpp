#include "d3dapp.h"
#include "errors.h"

#include <dxgi1_2.h>

using debug_utils::debug_mode;

D3DApp::D3DApp(HINSTANCE hinstance)
    : _hinstance(hinstance)
    , _hwnd(nullptr)
    , _is_paused(false)
    , _is_minimized(false)
    , _is_maximized(false)
    , _is_resizing(false)
    , _m4x_msaa_quality(0)
    , _window_width(800)
    , _window_height(600)
    , _enable_4x_msaa(false)
    , _d3d_device(nullptr)
    , _d3d_immediate_context(nullptr)
    , _swap_chain(nullptr)
    , _depth_stencil_buffer(nullptr)
    , _render_target_view(nullptr)
    , _depth_stencil_view(nullptr)
    , _screen_viewport({})
{
}

D3DApp::~D3DApp()
{
}

HINSTANCE D3DApp::hinstance() const
{
	return _hinstance;
}

HWND D3DApp::hwnd() const
{
	return _hwnd;
}

float D3DApp::aspect_ratio() const
{
	return static_cast<float>(_window_width) / _window_height;
}

std::optional<std::string> D3DApp::run()
{
	std::optional<std::string> err = std::nullopt;
	MSG                        msg = {0};

	_game_timer.reset();

	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			continue;
		}
		_game_timer.tick();
		calculate_frame_stats();
		update_scene(0);
		err = draw_scene();

		if (err)
			return err;
	}

	if (msg.wParam == 0)
		return std::nullopt;

	return fmt::format("app failed with error code: {}", msg.wParam);
}

std::optional<std::string> D3DApp::init()
{
	std::optional<std::string> err = std::nullopt;

	err = init_main_window();

	if (err)
		return err;

	err = init_direct_3d();

	return err;
}

void D3DApp::pause()
{
	_is_paused = true;
	_game_timer.stop();
}

void D3DApp::resume()
{
	_is_paused = false;
	_game_timer.start();
}

void D3DApp::on_resize(int w, int h)
{
	assert(w > 0 && h > 0);

	_window_width  = w;
	_window_height = h;

	if (_swap_chain == nullptr) {
		logger::log_info("on resize: swap chain is not initialized yet!");
		return;
	}

	_swap_chain->ResizeBuffers(1, w, h, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
}

void D3DApp::on_mouse_down(WPARAM, int, int)
{
}

void D3DApp::on_mouse_up(WPARAM, int, int)
{
}

void D3DApp::on_mouse_move(WPARAM, int, int)
{
}

std::optional<std::string> D3DApp::init_main_window()
{
	constexpr TCHAR appName[] = TEXT("DirectX Playground");

	WNDCLASS wndclass;
	wndclass.style         = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc   = D3DApp::wnd_proc;
	wndclass.cbClsExtra    = 0;
	wndclass.cbWndExtra    = 0;
	wndclass.hInstance     = _hinstance;
	wndclass.hIcon         = LoadIcon(nullptr, IDI_APPLICATION);
	wndclass.hCursor       = LoadCursor(nullptr, IDC_ARROW);
	wndclass.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wndclass.lpszMenuName  = nullptr;
	wndclass.lpszClassName = appName;

	if (!RegisterClass(&wndclass)) {
		return std::string("Register Class Failed!");
	}

	_hwnd = CreateWindow(
	    appName,
	    appName,
	    WS_OVERLAPPEDWINDOW,
	    CW_USEDEFAULT,
	    CW_USEDEFAULT,
	    _window_width,
	    _window_height,
	    nullptr,
	    nullptr,
	    _hinstance,
	    reinterpret_cast<void*>(this));

	ShowWindow(_hwnd, true);
	UpdateWindow(_hwnd);

	return std::nullopt;
}

std::optional<std::string> D3DApp::init_direct_3d()
{
	D3D_FEATURE_LEVEL feature_level;
	HRESULT           hr;
	UINT              create_device_flags = 0;

	if constexpr (debug_mode) {
		create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
	}

	hr = D3D11CreateDevice(
	    0,
	    D3D_DRIVER_TYPE_HARDWARE,
	    0,
	    create_device_flags,
	    0,
	    0,
	    D3D11_SDK_VERSION,
	    &_d3d_device,
	    &feature_level,
	    &_d3d_immediate_context);

	if (FAILED(hr)) {
		auto sys_err = debug_utils::get_system_error(hr);
		return fmt::format("D3D11CreateDevice error: {}", sys_err);
	}

	if (feature_level != D3D_FEATURE_LEVEL_11_0) {
		return std::string("Direct3D feature level 11 unsupported!");
	}

	_d3d_device->CheckMultisampleQualityLevels(
	    DXGI_FORMAT_R8G8B8A8_UNORM, 4, &_m4x_msaa_quality);

	if (FAILED(hr)) {
		auto sys_err = debug_utils::get_system_error(hr);
		return fmt::format("CheckMultisampleQualityLevels error: {}", sys_err);
	}

	assert(_m4x_msaa_quality > 0);

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width                   = _window_width;
	sd.BufferDesc.Height                  = _window_height;
	sd.BufferDesc.RefreshRate.Numerator   = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;

	if (_enable_4x_msaa) {
		sd.SampleDesc.Count   = 4;
		sd.SampleDesc.Quality = _m4x_msaa_quality;
	} else {
		sd.SampleDesc.Count   = 1;
		sd.SampleDesc.Quality = 0;
	}

	sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount  = 1;
	sd.OutputWindow = _hwnd;
	sd.Windowed     = true;
	sd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags        = 0;

	ComPtr<IDXGIDevice2> dxgi_device;
	hr = _d3d_device->QueryInterface(
	    __uuidof(IDXGIDevice2), reinterpret_cast<void**>(&dxgi_device));

	if (FAILED(hr)) {
		auto sys_err = debug_utils::get_system_error(hr);
		return fmt::format("Query Interface for IDXGIDevice2 failed: {}", sys_err);
	}

	hr = dxgi_device->SetMaximumFrameLatency(1);

	if (FAILED(hr)) {
		auto sys_err = debug_utils::get_system_error(hr);
		return fmt::format("IDXGIDevice set max frame latency failed: {}", sys_err);
	}

	ComPtr<IDXGIAdapter> dxgi_adapter;
	hr = dxgi_device->GetParent(
	    __uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgi_adapter));

	if (FAILED(hr)) {
		auto sys_err = debug_utils::get_system_error(hr);
		return fmt::format("dxgi_device->GetParent error: {}", sys_err);
	}

	ComPtr<IDXGIFactory> dxgi_factory;
	hr = dxgi_adapter->GetParent(
	    __uuidof(IDXGIFactory), reinterpret_cast<void**>(&dxgi_factory));

	if (FAILED(hr)) {
		auto sys_err = debug_utils::get_system_error(hr);
		return fmt::format("dxgi_adapter->GetParent error: {}", sys_err);
	}

	hr = dxgi_factory->CreateSwapChain(_d3d_device, &sd, &_swap_chain);

	if (FAILED(hr)) {
		auto sys_err = debug_utils::get_system_error(hr);
		return fmt::format("CreateSwapChain error: {}", sys_err);
	}

	ComPtr<ID3D11Texture2D> back_buffer;
	hr = _swap_chain->GetBuffer(
	    0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&back_buffer));

	if (FAILED(hr)) {
		auto sys_err = debug_utils::get_system_error(hr);
		return fmt::format("GetBuffer error: {}", sys_err);
	}

	hr = _d3d_device->CreateRenderTargetView(back_buffer, 0, &_render_target_view);

	if (FAILED(hr)) {
		auto sys_err = debug_utils::get_system_error(hr);
		return fmt::format("CreateRenderTargetView error: {}", sys_err);
	}

	D3D11_TEXTURE2D_DESC depth_stencil_desc;
	depth_stencil_desc.Width     = _window_width;
	depth_stencil_desc.Height    = _window_height;
	depth_stencil_desc.MipLevels = 1;
	depth_stencil_desc.ArraySize = 1;
	depth_stencil_desc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;

	if (_enable_4x_msaa) {
		depth_stencil_desc.SampleDesc.Count   = 4;
		depth_stencil_desc.SampleDesc.Quality = _m4x_msaa_quality;
	} else {
		depth_stencil_desc.SampleDesc.Count   = 1;
		depth_stencil_desc.SampleDesc.Quality = 0;
	}

	depth_stencil_desc.Usage          = D3D11_USAGE_DEFAULT;
	depth_stencil_desc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
	depth_stencil_desc.CPUAccessFlags = 0;
	depth_stencil_desc.MiscFlags      = 0;

	hr = _d3d_device->CreateTexture2D(&depth_stencil_desc, 0, &_depth_stencil_buffer);

	if (FAILED(hr)) {
		auto sys_err = debug_utils::get_system_error(hr);
		return fmt::format("CreateTexture2D error: {}", sys_err);
	}

	hr = _d3d_device->CreateDepthStencilView(
	    _depth_stencil_buffer, 0, &_depth_stencil_view);

	if (FAILED(hr)) {
		auto sys_err = debug_utils::get_system_error(hr);
		return fmt::format("CreateDepthStencilView error: {}", sys_err);
	}

	_d3d_immediate_context->OMSetRenderTargets(
	    1, &_render_target_view, _depth_stencil_view);

	_screen_viewport.TopLeftX = 0.0f;
	_screen_viewport.TopLeftY = 0.0f;
	_screen_viewport.Width    = static_cast<float>(_window_width);
	_screen_viewport.Height   = static_cast<float>(_window_height);
	_screen_viewport.MinDepth = D3D11_MIN_DEPTH;
	_screen_viewport.MaxDepth = D3D11_MAX_DEPTH;

	_d3d_immediate_context->RSSetViewports(1, &_screen_viewport);

	return std::nullopt;
}

void D3DApp::calculate_frame_stats()
{
	static int   frame_count  = 0;
	static float time_elapsed = 0.0f;

	frame_count++;

	if (_game_timer.total_time() - time_elapsed >= 1.0f) {
		float fps  = static_cast<float>(frame_count);
		float mspf = 1000.f / fps;

		frame_count = 0;
		time_elapsed += 1.0f;
	}
}

LRESULT CALLBACK D3DApp::wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static D3DApp* d3dapp = nullptr;

	try {
		switch (msg) {
		case WM_CREATE: {
			auto* create_params = reinterpret_cast<CREATESTRUCT*>(lParam);
			d3dapp = reinterpret_cast<D3DApp*>(create_params->lpCreateParams);

			return 0;
		}

		case WM_ACTIVATE:
			if (LOWORD(wParam) == WA_INACTIVE) {
				d3dapp->pause();
			} else {
				d3dapp->resume();
			}

			return 0;

		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			d3dapp->on_mouse_down(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;

		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			d3dapp->on_mouse_up(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;

		case WM_MOUSEMOVE:
			d3dapp->on_mouse_move(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;

		case WM_SIZE:
			d3dapp->on_resize(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);

	} catch (LogError& e) {
		// Do not call logger::log. We assume the logger will dump the
		// error if possible.
		MessageBoxA(hwnd, e.what(), "Fatal error!", MB_OK | MB_ICONERROR);

		PostQuitMessage(0);
		return 0;
	} catch (std::exception& e) {
		MessageBoxA(hwnd, e.what(), "Fatal error!", MB_OK | MB_ICONERROR);
		logger::log_fatal(e.what());

		PostQuitMessage(0);
		return 0;
	}
}
