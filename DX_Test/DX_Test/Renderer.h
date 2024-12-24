#pragma once
#include <wrl.h>
#include <d3d11_4.h> 
#include <d3dcompiler.h> 

#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler") 
#pragma comment(lib, "dxguid")

#pragma comment(lib, "DXGI") 

// Ό³Έν :
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

	void BeginPlay();
	void Render();
	void Release();


protected:

private:

};

