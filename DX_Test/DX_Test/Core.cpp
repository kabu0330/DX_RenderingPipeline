#include "PreCompile.h"
#include "Core.h"
#include "Renderer.h"
#include <iostream>
#include "Level.h"
#include "GameMode.h"

UGraphicsDevice UCore::GraphicsDevice;
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
	WindowSize = { 1280, 900 };
	SetWindowPosAndScale({ 100, 100 }, WindowSize);

	GraphicsDevice.Init();
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
	GraphicsDevice.Release();

	CurLevel->Release();
	delete CurLevel;
}

