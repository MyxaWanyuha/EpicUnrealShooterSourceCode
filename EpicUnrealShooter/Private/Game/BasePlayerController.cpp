// Copyright Saber Studio. All Rights Reserved.


#include "Game/BasePlayerController.h"
#include "Game/BaseGameMode.h"

DEFINE_LOG_CATEGORY_STATIC(LogBasePlayerController, All, All);

void ABasePlayerController::LaunchRespawnTimer(float RespawnTime)
{
	if(auto World = GetWorld())
	{
		World->GetTimerManager().SetTimer(RespawnTimer, this, &ABasePlayerController::SpawnPlayer, RespawnTime, false);
	}
}

void ABasePlayerController::SpawnPlayer()
{
	if(auto World = GetWorld())
	{
		if(auto GM = Cast<ABaseGameMode>(World->GetAuthGameMode()))
		{
			GM->SpawnPlayer(this);
		}
	}
}
