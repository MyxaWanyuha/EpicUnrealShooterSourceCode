// Copyright Saber Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PickUpItems/PickUpBase.h"
#include "PickUpArmor.generated.h"

UCLASS()
class EPICUNREALSHOOTER_API APickUpArmor : public APickUpBase
{
	GENERATED_BODY()
public:
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};
