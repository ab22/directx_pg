#include <DirectXColors.h>
#include <d3dcompiler.h>
#include <fstream>
#include <math.h>

#include "debug_utils.h"
#include "demo_app.h"
#include "logger.h"
#include "vertex.h"

using namespace DirectX;

DemoApp::DemoApp(HINSTANCE hInstance)
    : D3DApp(hInstance)
    , _box_vertex_buffer(nullptr)
    , _box_index_buffer(nullptr)
    , _input_layout(nullptr)
    , _theta(0.f)
    , _phi(0.f)
    , _radius(15.f)
{
	XMMATRIX identity = XMMatrixIdentity();
	_last_mouse_pos.x = 0;
	_last_mouse_pos.y = 0;
	_theta            = 1.5f * XM_PI;
	_phi              = 0.25f * XM_PI;

	XMStoreFloat4x4(&_world, identity);
	XMStoreFloat4x4(&_view, identity);
	XMStoreFloat4x4(&_proj, identity);
}

DemoApp::~DemoApp()
{
}

std::optional<std::string> DemoApp::init()
{
	std::optional<std::string> err = std::nullopt;

	err = D3DApp::init();
	if (err)
		return err;

	err = build_geometry_buffers();
	if (err)
		return err;

	err = build_fx();
	return err;
}

void DemoApp::on_resize(int w, int h)
{
	D3DApp::on_resize(w, h);

	XMMATRIX persp = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect_ratio(), 1.f, 1000.f);
	XMStoreFloat4x4(&_proj, persp);
}

void DemoApp::update_scene(float)
{
	// Convert spherical to cartesian coords.
	float x = _radius * XMScalarSin(_phi) * XMScalarCos(_theta);
	float z = _radius * XMScalarSin(_phi) * XMScalarSin(_theta);
	float y = _radius * XMScalarCos(_phi);

	// Build view matrix.
	XMVECTOR pos    = XMVectorSet(x, y, z, 1.f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up     = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&_view, V);
}

std::optional<std::string> DemoApp::draw_scene()
{
	assert(_d3d_immediate_context != nullptr);
	assert(_swap_chain != nullptr);

	HRESULT hr;

	_d3d_immediate_context->ClearRenderTargetView(
	    _render_target_view, Colors::BlueViolet);
	_d3d_immediate_context->ClearDepthStencilView(
	    _depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	_d3d_immediate_context->IASetInputLayout(_input_layout);

	UINT stride = sizeof(Vertex1);
	UINT offset = 0;

	_d3d_immediate_context->IASetVertexBuffers(
	    0, 1, &_box_vertex_buffer, &stride, &offset);
	_d3d_immediate_context->IASetIndexBuffer(_box_index_buffer, DXGI_FORMAT_R32_UINT, 0);

	// Set constants
	ConstantBuffer cb;
	XMMATRIX       world = XMLoadFloat4x4(&_world);
	XMMATRIX       view  = XMLoadFloat4x4(&_view);
	XMMATRIX       proj  = XMLoadFloat4x4(&_proj);
	XMMATRIX       wvp   = world * view * proj;
	wvp                  = XMMatrixTranspose(wvp);

	XMStoreFloat4x4(&cb.wvp, wvp);

	_d3d_immediate_context->UpdateSubresource(_box_const_buffer, 0, nullptr, &cb, 0, 0);
	_d3d_immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_d3d_immediate_context->VSSetShader(_vertex_shader, nullptr, 0);
	_d3d_immediate_context->VSSetConstantBuffers(0, 1, &_box_const_buffer);
	_d3d_immediate_context->PSSetShader(_pixel_shader, nullptr, 0);
	_d3d_immediate_context->RSSetState(_rasterizer_state);
	_d3d_immediate_context->DrawIndexed(36, 0, 0);

	hr = _swap_chain->Present(1, 0);

	if (FAILED(hr)) {
		auto sys_err = debug_utils::get_system_error(hr);
		return fmt::format("SwapChain Present error: {}", sys_err);
	}

	return std::nullopt;
}

std::optional<std::string> DemoApp::build_geometry_buffers()
{
	assert(_d3d_device != nullptr);

	// Vertex buffer
	Vertex1 vertices[] = {
	    {XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	    {XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
	    {XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
	    {XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
	    {XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)},
	    {XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f)},
	    {XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)},
	    {XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)},
	};

	D3D11_BUFFER_DESC vbd;
	vbd.Usage               = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth           = sizeof(Vertex1) * 8;
	vbd.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags      = 0;
	vbd.MiscFlags           = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vdata;
	vdata.pSysMem          = vertices;
	vdata.SysMemPitch      = 0;
	vdata.SysMemSlicePitch = 0;

	auto hr = _d3d_device->CreateBuffer(&vbd, &vdata, &_box_vertex_buffer);

	if (FAILED(hr)) {
		auto sys_err = debug_utils::get_system_error(hr);
		return fmt::format("Create vertex buffer failed: {}", sys_err);
	}

	// Index buffer
	// clang-format off
	UINT indices[] =
    {
		// front face
		0, 1, 2,
		0, 2, 3,
	    // back face
	    4, 6, 5,
		4, 7, 6,
	    // left face
	    4, 5, 1,
	    4, 1, 0,
	    // right face
	    3, 2, 6,
	    3, 6, 7,
	    // top face
	    1, 5, 6,
	    1, 6, 2,
	    // bottom face
	    4, 0, 3,
	    4, 3, 7
    };
	// clang-format on

	D3D11_BUFFER_DESC ibd;
	ibd.Usage               = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth           = sizeof(UINT) * 36;
	ibd.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags      = 0;
	ibd.MiscFlags           = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA idata;
	idata.pSysMem          = indices;
	idata.SysMemPitch      = 0;
	idata.SysMemSlicePitch = 0;

	hr = _d3d_device->CreateBuffer(&ibd, &idata, &_box_index_buffer);

	if (FAILED(hr)) {
		auto sys_err = debug_utils::get_system_error(hr);
		return fmt::format("Create index buffer failed: {}", sys_err);
	}

	D3D11_BUFFER_DESC cbd   = {0};
	cbd.ByteWidth           = sizeof(ConstantBuffer);
	cbd.Usage               = D3D11_USAGE_DEFAULT;
	cbd.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags      = 0;
	cbd.MiscFlags           = 0;
	cbd.StructureByteStride = 0;

	hr = _d3d_device->CreateBuffer(&cbd, nullptr, &_box_const_buffer);

	if (FAILED(hr)) {
		auto sys_err = debug_utils::get_system_error(hr);
		return fmt::format("Create constant buffer failed: {}", sys_err);
	}

	D3D11_RASTERIZER_DESC rd;
	memset(&rd, 0, sizeof(rd));
	rd.CullMode = D3D11_CULL_FRONT;
	rd.FillMode = D3D11_FILL_SOLID;

	hr = _d3d_device->CreateRasterizerState(&rd, &_rasterizer_state);

	if (FAILED(hr)) {
		auto sys_err = debug_utils::get_system_error(hr);
		return fmt::format("Failed to create rasterizer state: {}", sys_err);
	}

	return std::nullopt;
}

tl::expected<std::vector<std::byte>, std::string>
DemoApp::read_fx_from_file(std::string_view filename)
{
	if (filename.empty()) {
		auto msg = fmt::format("failed to read fx file: empty filename specfied!");
		return tl::make_unexpected(msg);
	}

	std::ifstream file(filename.data(), std::ifstream::binary | std::ifstream::ate);

	if (!file.is_open()) {
		auto msg = fmt::format("failed to open file: {}", filename);
		return tl::make_unexpected(msg);
	}

	auto file_size = file.tellg();
	file.seekg(0, std::ifstream::beg);

	std::vector<std::byte> file_data;
	file_data.resize(file_size);

	file.read(reinterpret_cast<char*>(file_data.data()), file_size);

	if (!file) {
		auto msg = fmt::format(
		    "failed to read from {}: only {} bytes could be read",
		    filename,
		    file.gcount());
		return tl::make_unexpected(msg);
	}

	return file_data;
}

std::optional<std::string> DemoApp::build_fx()
{
	HRESULT hr = 0;

	auto vs_result = read_fx_from_file("vertex1_vs.cso");
	if (!vs_result)
		return vs_result.error();

	auto vs_bytecode = vs_result.value();
	hr               = _d3d_device->CreateVertexShader(
        vs_bytecode.data(), vs_bytecode.size(), nullptr, &_vertex_shader);

	if (FAILED(hr)) {
		auto sys_msg = debug_utils::get_system_error(hr);
		auto msg     = fmt::format("failed to create vertex shader: {}", sys_msg);

		return msg;
	}

	auto err = build_vertex_layout(vs_bytecode);
	if (err)
		return err;

	auto ps_result = read_fx_from_file("vertex1_ps.cso");
	if (!ps_result)
		return ps_result.error();

	auto ps_bytecode = ps_result.value();
	hr               = _d3d_device->CreatePixelShader(
        ps_bytecode.data(), ps_bytecode.size(), nullptr, &_pixel_shader);

	if (FAILED(hr)) {
		auto sys_msg = debug_utils::get_system_error(hr);
		auto msg     = fmt::format("failed to create pixel shader: {}", sys_msg);

		return msg;
	}

	return std::nullopt;
}

std::optional<std::string> DemoApp::build_vertex_layout(std::vector<std::byte>& shader)
{
	// clang-format off
	D3D11_INPUT_ELEMENT_DESC vertex_desc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	// clang-format on

	HRESULT hr = _d3d_device->CreateInputLayout(
	    vertex_desc, 2, shader.data(), shader.size(), &_input_layout);

	if (FAILED(hr)) {
		auto sys_msg = debug_utils::get_system_error(hr);
		auto msg     = fmt::format("create input layout failed: {}", sys_msg);

		return msg;
	}

	return std::nullopt;
}

void DemoApp::on_mouse_down(WPARAM, int x, int y)
{
	_last_mouse_pos.x = x;
	_last_mouse_pos.y = y;

	SetCapture(_hwnd);
}

void DemoApp::on_mouse_up(WPARAM, int, int)
{
	ReleaseCapture();
}

void DemoApp::on_mouse_move(WPARAM state, int x, int y)
{
	if ((state & MK_LBUTTON) != 0) {
		float dx = XMConvertToRadians(.25f * static_cast<float>(x - _last_mouse_pos.x));
		float dy = XMConvertToRadians(.25f * static_cast<float>(y - _last_mouse_pos.y));

		// Update angles based on input to orbit camera around box.
		_theta += dx;
		_phi += dy;

		// Restrict the angle phi.
		_phi = min(XM_PI - 0.1f, max(_phi, 0.1f));
	} else if ((state & MK_RBUTTON) != 0) {
		// Make each pixel correspond to 0.005 unit in the scene.
		float dx = 0.1f * static_cast<float>(x - _last_mouse_pos.x);
		float dy = 0.1f * static_cast<float>(y - _last_mouse_pos.y);

		// Update camera radius based on input.
		_radius += dx - dy;

		// Restrict the radius.
		_radius = min(15.f, max(_radius, 3.f));
	}

	_last_mouse_pos.x = x;
	_last_mouse_pos.y = y;
}
