#include "demo_app.h"

DemoApp::DemoApp(HINSTANCE hInstance)
	: D3DApp(hInstance)
{

}

DemoApp::~DemoApp()
{
}

bool DemoApp::init()
{
	return D3DApp::init();
}

void DemoApp::on_resize(int w, int h)
{
	D3DApp::on_resize(w, h);
}

void DemoApp::update_scene(float dt)
{

}

void DemoApp::draw_scene()
{
	assert(_d3d_immediate_context != nullptr);
	assert(_swap_chain != nullptr);

	HRESULT hr;
	float   color = 0.0f;


	_d3d_immediate_context->ClearRenderTargetView(_render_target_view, &color);
	_d3d_immediate_context->ClearDepthStencilView(
		_depth_stencil_view,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	hr = _swap_chain->Present(0,0);

	if (FAILED(hr)) {
		logger::log_dx_error(hr, "SwapChain Present error");
		return;
	}
}
