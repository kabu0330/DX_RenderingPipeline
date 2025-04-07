#include "PreCompile.h"
#include "Actor.h"

std::list<URenderer*> AActor::Renderers;

AActor::AActor()
{
}

AActor::~AActor()
{

}

void AActor::BeginPlay()
{
	for (URenderer* Renderer : BeginPlayRenderers)
	{
		Renderer->BeginPlay();
	}
	BeginPlayRenderers.clear();
}

void AActor::Render(float _DeltaTime)
{
	for (URenderer* Renderer : Renderers)
	{
		Renderer->Render(_DeltaTime);
	}
}

void AActor::ComponentBeginPlay()
{
	for (URenderer* Renderer : Renderers)
	{
		Renderer->BeginPlay();
	}
}

void AActor::Release()
{
	for (URenderer* Renderer : Renderers)
	{
		Renderer->Release();
		delete Renderer;
	}
	Renderers.clear();
}
