// Copyright Saber Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "ShooterGameSession.generated.h"

UCLASS()
class EPICUNREALSHOOTER_API AShooterGameSession : public AGameSession
{
	GENERATED_BODY()
	
	virtual void RegisterServer() override;
};
