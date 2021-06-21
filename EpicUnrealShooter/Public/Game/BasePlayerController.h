// Copyright Saber Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

UCLASS()
class EPICUNREALSHOOTER_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	void LaunchRespawnTimer(float RespawnTime);
	void SpawnPlayer();

protected:
	FTimerHandle RespawnTimer;
};
