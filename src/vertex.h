#pragma once

#include <DirectXMath.h>

struct Vertex1 {
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
};

struct Vertex2 {
	DirectX::XMFLOAT2 position;
};

struct ConstantBuffer {
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX proj;
};
