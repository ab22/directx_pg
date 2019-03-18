cbuffer cbPerObject
{
	matrix world;
	matrix view;
	matrix proj;
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

	pos = mul(pos, world);
	pos = mul(pos, view);
	pos = mul(pos, proj);

	output.pos_h = pos;
	output.color = input.color;

	return output;
}

/*
struct VertexShaderInput
{
    float2 pos : POSITION;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
};

PixelShaderInput SimpleVertexShader(VertexShaderInput input)
{
    PixelShaderInput vertexShaderOutput;

    // For this lesson, set the vertex depth value to 0.5 so it is guaranteed to be drawn.
    vertexShaderOutput.pos = float4(input.pos, 0.5f, 1.0f);

    return vertexShaderOutput;
}*/
