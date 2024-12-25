#pragma once

#include <Base/EngineMath.h>

#include <d3d11_4.h> 
#include <d3dcompiler.h> 

#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler") 
#pragma comment(lib, "dxguid")

#pragma comment(lib, "DXGI") 

struct VertexData
{
	float4 POSITION;
	float4 TEXCOORD;
	float4 COLOR;
};

// ���� :
class URenderer
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

	void CreateDeviceAndContext();
	void CreateBackBuffer();

	void RenderStart();
	void RenderEnd();
	virtual void Render(float _DeltaTime);

	virtual void BeginPlay();
	virtual	void Release();

	ID3D11Device* Device = nullptr;
	ID3D11DeviceContext* Context = nullptr;

protected:

private:
	IDXGIAdapter* MainAdapter = nullptr;
	IDXGISwapChain* SwapChain = nullptr;

	ID3D11Texture2D* BackBufferTexture = nullptr;
	ID3D11RenderTargetView* RTV = nullptr;

private:
	// �������
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

