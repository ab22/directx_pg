#pragma once

#include <DirectXMath.h>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "com_ptr.h"
#include "d3dapp.h"
#include "tl/expected.hpp"

class DemoApp: public D3DApp {
  public:
	DemoApp(HINSTANCE);
	~DemoApp();

	std::optional<std::string> init() override;

	void                       on_resize(int w, int h) override;
	void                       update_scene(float dt) override;
	std::optional<std::string> draw_scene() override;

  protected:
	virtual void on_mouse_down(WPARAM, int x, int y);
	virtual void on_mouse_up(WPARAM, int x, int y);
	virtual void on_mouse_move(WPARAM, int x, int y);

  private:
	ComPtr<ID3D11VertexShader>    _vertex_shader;
	ComPtr<ID3D11PixelShader>     _pixel_shader;
	ComPtr<ID3D11Buffer>          _box_vertex_buffer;
	ComPtr<ID3D11Buffer>          _box_index_buffer;
	ComPtr<ID3D11Buffer>          _box_const_buffer;
	ComPtr<ID3D11InputLayout>     _input_layout;
	ComPtr<ID3D11RasterizerState> _rasterizer_state;
	DirectX::XMFLOAT4X4           _world;
	DirectX::XMFLOAT4X4           _view;
	DirectX::XMFLOAT4X4           _proj;
	float                         _theta;
	float                         _phi;
	float                         _radius;
	POINT                         _last_mouse_pos;

	std::optional<std::string> build_geometry_buffers();
	std::optional<std::string> build_fx();
	std::optional<std::string> build_vertex_layout(std::vector<std::byte>& shader);
	tl::expected<std::vector<std::byte>, std::string> read_fx_from_file(std::string_view);
};
