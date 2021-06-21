// Copyright Saber Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FireHole.generated.h"

UCLASS()
class EPICUNREALSHOOTER_API AFireHole : public AActor
{
	GENERATED_BODY()
	
public:
	AFireHole();

protected:
	virtual void BeginPlay() override;
	
};
