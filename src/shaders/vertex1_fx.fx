cbuffer cbPerObject : register(b0)
{
	matrix wvp;
}

struct VertexIn
{
	float3 position: POSITION;
	float4 color: COLOR;
};

struct VertexOut
{
	float4 pos_h: SV_POSITION;
	float4 color: COLOR;
};

VertexOut vertex_shader(VertexIn input)
{
	VertexOut output;
	float4 pos = float4(input.position, 1.0f);
	output.pos_h = mul(pos, wvp);
	output.color = input.color;

	return output;
}

float4 pixel_shader(VertexOut input) : SV_TARGET
{
	return input.color;
}

RasterizerState WireframeRS
{
	FillMode = Wireframe;
	CullMode = Back;
	FrontCounterClockwise = false;
};

technique11 ColorTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, vertex_shader()));
		SetPixelShader(CompileShader(ps_5_0, pixel_shader()));

		SetRasterizerState(WireframeRS);
	}
}
