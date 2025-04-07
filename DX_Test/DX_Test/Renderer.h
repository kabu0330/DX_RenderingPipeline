#pragma once

#include <Base/EngineMath.h>
#include "Actor.h"

struct VertexData
{
	float4 POSITION;
	float4 TEXCOORD;
	float4 COLOR;
};

// 설명 :
class URenderer : public AActor
{
public:
	// constrcuter destructer
	URenderer();
	~URenderer();

	// delete Function
	URenderer(const URenderer& _Other) = delete;
	URenderer(URenderer&& _Other) noexcept = delete;
	URenderer& operator=(const URenderer& _Other) = delete;
	URenderer& operator=(URenderer&& _Other) noexcept = delete;

	virtual void Render(float _DeltaTime);

	void BeginPlay() override;
	void Release() {}

protected:

private:


private:
	// WVP Setting
	void WorldViewProjection();

	// 상수버퍼
	ID3D11SamplerState* SamplerState = nullptr;
	ID3D11Buffer* TransformConstBuffer = nullptr;
	void ShaderResInit();
	void ShaderResSetting();

	// Level 1 : Iuput Assembler
	ID3D11Buffer* VertexBuffer = nullptr;
	ID3D11InputLayout* InputLayout = nullptr;
	void InputAssembler1Init();
	void InputAssembler1Layout();
	void InputAssembler1Setting();

	ID3D11Buffer* IndexBuffer = nullptr;
	D3D11_PRIMITIVE_TOPOLOGY Topology = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	void InputAssembler2Init();
	void InputAssembler2Setting();

	// Level 2 : Vertex Shader
	ID3DBlob* VSShaderCodeBlob = nullptr;
	ID3DBlob* VSErrorCodeBlob = nullptr;
	ID3D11VertexShader* VertexShader = nullptr;
	void VertexShaderInit();
	void VertexShaderSetting();

	// Level 3: Rasterizer
	D3D11_VIEWPORT ViewportInfo;
	ID3D11RasterizerState* RasterizerState = nullptr;
	void RasterizerInit();
	void RasterizerSetting();

	// Level 4 : Pixel Shader
	ID3DBlob* PSShaderCodeBlob = nullptr;
	ID3DBlob* PSErrorCodeBlob = nullptr;
	ID3D11PixelShader* PixelShader = nullptr;
	void PixelShaderInit();
	void PixelShaderSetting();
	
	// Level 5 : Output Merger 
	void OutputMergeSetting();




};

