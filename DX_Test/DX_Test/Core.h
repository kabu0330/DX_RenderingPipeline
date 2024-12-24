#pragma once
#include "Renderer.h"

// Ό³Έν :
class Core
{
public:
	// constrcuter destructer
	Core();
	~Core();

	// delete Function
	Core(const Core& _Other) = delete;
	Core(Core&& _Other) noexcept = delete;
	Core& operator=(const Core& _Other) = delete;
	Core& operator=(Core&& _Other) noexcept = delete;

	static void Start();
	static void Tick();
	static void End();



protected:

private:
	static URenderer Renderer;
};

