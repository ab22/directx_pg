
struct VertexOut
{
	float4 pos_h: SV_POSITION;
	float4 color: COLOR;
};

float4 pixel_shader(VertexOut input) : SV_TARGET
{
	return input.color;
}

/*
struct PixelShaderInput
{
    float4 pos : SV_POSITION;
};

float4 SimplePixelShader(PixelShaderInput input) : SV_TARGET
{
    // Draw the entire triangle yellow.
    return float4(1.0f, 1.0f, 0.0f, 1.0f);
}
*/
