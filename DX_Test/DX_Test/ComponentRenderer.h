#pragma once
#include "Renderer.h"

// Ό³Έν :
class UComponentRenderer : public URenderer
{
public:
	// constrcuter destructer
	UComponentRenderer();
	~UComponentRenderer();

	// delete Function
	UComponentRenderer(const UComponentRenderer& _Other) = delete;
	UComponentRenderer(UComponentRenderer&& _Other) noexcept = delete;
	UComponentRenderer& operator=(const UComponentRenderer& _Other) = delete;
	UComponentRenderer& operator=(UComponentRenderer&& _Other) noexcept = delete;

	void BeginPlay() override {}
	void Render(float _DeltaTime) override {}
	void Release() override {}

protected:

private:

};

