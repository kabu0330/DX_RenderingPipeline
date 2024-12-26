#pragma once

#include <d3d11_4.h> 
#include <d3dcompiler.h> 

#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler") 
#pragma comment(lib, "dxguid")

#pragma comment(lib, "DXGI") 

// Ό³Έν :
class UGraphicsDevice
{
public:
	// constrcuter destructer
	UGraphicsDevice();
	~UGraphicsDevice();

	// delete Function
	UGraphicsDevice(const UGraphicsDevice& _Other) = delete;
	UGraphicsDevice(UGraphicsDevice&& _Other) noexcept = delete;
	UGraphicsDevice& operator=(const UGraphicsDevice& _Other) = delete;
	UGraphicsDevice& operator=(UGraphicsDevice&& _Other) noexcept = delete;

	void Init();
	void CreateDeviceAndContext();
	void CreateBackBuffer();

	void RenderStart();

	void RenderEnd();

	void Release();

	ID3D11Device* Device = nullptr;
	ID3D11DeviceContext* Context = nullptr;
	ID3D11RenderTargetView* RTV = nullptr;

protected:

private:
	IDXGIAdapter* MainAdapter = nullptr;
	IDXGISwapChain* SwapChain = nullptr;

	ID3D11Texture2D* BackBufferTexture = nullptr;
};

