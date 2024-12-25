#include "PreCompile.h"
#include "Core.h"
#include "Renderer.h"
#include <iostream>
#include "Level.h"
#include "GameMode.h"

URenderer UCore::Renderer;
FVector UCore::WindowSize = { 100, 100 };
UEngineTimer UCore::Timer;
ULevel* UCore::CurLevel;

UCore::UCore()
{
}

UCore::~UCore()
{
}

void UCore::Start()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	WindowSize = { 1280, 900 };
	SetWindowPosAndScale({ 100, 100 }, WindowSize);

	Renderer.BeginPlay();
	CreateLevel<AGameMode>();
}

void UCore::Tick()
{
	Timer.TimeCheck();
	float DeltaTime = Timer.GetDeltaTime();
	CurLevel->Tick(DeltaTime);
	CurLevel->Render(DeltaTime);
}

void UCore::End()
{
	Renderer.Release();
}

