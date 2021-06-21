// Copyright Saber Studio. All Rights Reserved.


#include "Game/BasePlayerState.h"

#include "Engine/DemoNetDriver.h"
#include "Game/BaseGameState.h"
#include "Kismet/GameplayStatics.h"

void ABasePlayerState::BeginPlay()
{
}

void ABasePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABasePlayerState, Kills);
	DOREPLIFETIME(ABasePlayerState, Deaths);
	DOREPLIFETIME(ABasePlayerState, bIsAlive);
}

void ABasePlayerState::AddDeath()
{
	++Deaths;
}

void ABasePlayerState::AddKill()
{
	++Kills;
}

void ABasePlayerState::OnRep_bIsAlive()
{
	if(bIsAlive)
	{
		OnPlayerAlive.Broadcast();
	}
}

void ABasePlayerState::OwnerCharDead_Implementation(APlayerCharacter* LInstigator, AActor* DeathCauser)
{
	bIsAlive = false;
	OnPlayerDead.Broadcast(LInstigator, DeathCauser, this);
}
