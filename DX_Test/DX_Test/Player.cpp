#include "PreCompile.h"
#include "Player.h"

APlayer::APlayer()
{
	PlayerRenderer = CreateDefaultSubObject<URenderer>();
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

