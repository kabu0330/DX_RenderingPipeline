#pragma once
#include "Actor.h"

// Ό³Έν :
class AGameMode : public AActor
{
public:
	// constrcuter destructer
	AGameMode();
	~AGameMode();

	// delete Function
	AGameMode(const AGameMode& _Other) = delete;
	AGameMode(AGameMode&& _Other) noexcept = delete;
	AGameMode& operator=(const AGameMode& _Other) = delete;
	AGameMode& operator=(AGameMode&& _Other) noexcept = delete;

	void BeginPlay() override;
	void Tick(float _DeltaTime) override;

protected:

private:
	class APlayer* Rect = nullptr;
};

