struct VertexData
{
	float4 POSITION : POSITION;
	//float4 TEXCOORD : TEXCOORD;
	float4 COLOR : COLOR;
};

struct VertexShaderOutput
{
	float4 SVPOSITION : SV_POSITION;
	//float4 TEXCOORD : TEXCOORD;
	float4 COLOR : COLOR;
};

VertexShaderOutput VertexToWorld(VertexData _Vertex)
{
	VertexShaderOutput Output;
	
	Output.SVPOSITION = _Vertex.POSITION;
	//Output.TEXCOORD   = _Vertex.TEXCOORD;
	Output.COLOR      = _Vertex.COLOR;
	
	return Output;

}

// --------------------------------------------------------------------

Texture2D ImageTexture : register(t0);
SamplerState ImageSampler : register(s0);

float4 PixelToWorld(VertexShaderOutput _Vertex) : SV_Target0
{
	//float4 Color = ImageTexture.Sample(ImageSampler, _Vertex.TEXCOORD.xy);
    float Color = _Vertex.COLOR;
	return Color;
}