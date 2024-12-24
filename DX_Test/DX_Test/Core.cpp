#include "Core.h"
#include "Renderer.h"
#include <iostream>

URenderer Core::Renderer;

Core::Core()
{
}

Core::~Core()
{
}

void Core::Start()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	Renderer.BeginPlay();
}

void Core::Tick()
{
	while (true)
	{
		Renderer.Render();
	}
}

void Core::End()
{
	Renderer.Release();
}

