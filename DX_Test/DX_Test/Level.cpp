#include "PreCompile.h"
#include "Level.h"

ULevel::ULevel()
{
}

ULevel::~ULevel()
{
}

void ULevel::BeginPlayCheck()
{
	for (AActor* Actor : BeginPlayActors)
	{
		Actor->BeginPlay();
	}
	BeginPlayActors.clear();
}

void ULevel::Tick(float _DeltaTime)
{
	BeginPlayCheck();

	for (AActor* Actor : AllActors)
	{
		Actor->Tick(_DeltaTime);
	}
}

void ULevel::Render(float _DeltaTime)
{
	UCore::Renderer.RenderStart();

	for (AActor* Actor : AllActors)
	{
		Actor->Render(_DeltaTime);
	}

	UCore::Renderer.RenderEnd();
}


