#include "PreCompile.h"
#include "GameMode.h"
#include "Player.h"
#include "Core.h"

AGameMode::AGameMode()
{
}

AGameMode::~AGameMode()
{
}

void AGameMode::BeginPlay()
{
	Super::BeginPlay();

	Rect = UCore::GetWorld()->SpawnActor<APlayer>();

}

void AGameMode::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);
}

