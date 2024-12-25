#include "PreCompile.h"
#include "Player.h"
#include "ComponentRenderer.h"

APlayer::APlayer()
{
}

APlayer::~APlayer()
{
}

void APlayer::BeginPlay()
{
	PlayerRenderer = CreateDefaultSubObject<UComponentRenderer>();

}

void APlayer::Tick(float _DeltaTime)
{
}

