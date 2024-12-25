#pragma once
#include "Renderer.h"
#include <Base/EngineTimer.h>
#include "Level.h"

// Ό³Έν :
class UCore
{
public:
	// constrcuter destructer
	UCore();
	~UCore();

	// delete Function
	UCore(const UCore& _Other) = delete;
	UCore(UCore&& _Other) noexcept = delete;
	UCore& operator=(const UCore& _Other) = delete;
	UCore& operator=(UCore&& _Other) noexcept = delete;

	static void Start();
	static void Tick();
	static void End();

	template<typename GameModeType>
	static ULevel* CreateLevel()
	{
		ULevel* NewLevel = new ULevel;

		NewLevel->SpawnActor<GameModeType>();

		CurLevel = NewLevel;
		return NewLevel;
	}

	static ULevel* GetWorld()
	{
		return CurLevel;
	}

	static URenderer Renderer;

	static FVector WindowSize;

protected:

private:
	static UEngineTimer Timer;
	static ULevel* CurLevel;
};

