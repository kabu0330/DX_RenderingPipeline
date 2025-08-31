#include "PreCompile.h"
#include "Player.h"

APlayer::APlayer()
{
	PlayerRenderer = CreateDefaultSubObject<URenderer>();
	SetActorRelativeScale3D({ 0.1f, 1.0f, 1.0f });
}

APlayer::~APlayer()
{
}

void APlayer::BeginPlay()
{
	Super::BeginPlay();
}

void APlayer::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

}

