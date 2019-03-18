#pragma once

#include "com_ptr.h"
#include "debug_utils.h"
#include "game_timer.h"

// Disable macro redefinition warning inside Direct 3D header files.
#pragma warning(push)
#pragma warning(disable : 4005)
#include <d3d11.h>
#pragma warning(pop)

#include <Windows.h>
#include <cassert>
#include <memory>
#include <optional>
#include <windowsx.h>

class D3DApp {
  public:
	D3DApp(HINSTANCE);
	virtual ~D3DApp();

	HINSTANCE hinstance() const;
	HWND      hwnd() const;
	float     aspect_ratio() const;

	std::optional<std::string> run();

	virtual std::optional<std::string> init();

	virtual void                       pause();
	virtual void                       resume();
	virtual void                       on_resize(int w, int h);
	virtual void                       update_scene(float dt) = 0;
	virtual std::optional<std::string> draw_scene()           = 0;
	virtual void                       on_mouse_down(WPARAM, int x, int y);
	virtual void                       on_mouse_up(WPARAM, int x, int y);
	virtual void                       on_mouse_move(WPARAM, int x, int y);

  protected:
	GameTimer _game_timer;
	HINSTANCE _hinstance;
	HWND      _hwnd;
	bool      _is_paused;
	bool      _is_minimized;
	bool      _is_maximized;
	bool      _is_resizing;
	UINT      _m4x_msaa_quality;
	int       _window_width;
	int       _window_height;
	bool      _enable_4x_msaa;

	ComPtr<ID3D11Device>           _d3d_device;
	ComPtr<ID3D11DeviceContext>    _d3d_immediate_context;
	ComPtr<IDXGISwapChain>         _swap_chain;
	ComPtr<ID3D11Texture2D>        _depth_stencil_buffer;
	ComPtr<ID3D11RenderTargetView> _render_target_view;
	ComPtr<ID3D11DepthStencilView> _depth_stencil_view;
	D3D11_VIEWPORT                 _screen_viewport;
	D3D_DRIVER_TYPE                _d3d_driver_type;

	std::optional<std::string> init_main_window();
	std::optional<std::string> init_direct_3d();
	void                       calculate_frame_stats();

  private:
	static LRESULT CALLBACK wnd_proc(HWND, UINT, WPARAM, LPARAM);
};
