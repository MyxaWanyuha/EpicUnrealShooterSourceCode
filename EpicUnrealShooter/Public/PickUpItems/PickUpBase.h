// Copyright Saber Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "PickUpBase.generated.h"

UCLASS()
class EPICUNREALSHOOTER_API APickUpBase : public AActor
{
	GENERATED_BODY()
public:
	APickUpBase();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CountToAdd = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ResetTime = 5.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* Effect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystemComponent* EffectComponent;
	
	UPROPERTY()
	UBoxComponent* CollisionComponent;

	UPROPERTY(ReplicatedUsing=OnRep_bIsVisible)
	bool bIsVisible = true;

	UPROPERTY(EditAnywhere, Category="Sound")
	USoundBase* PickUpSound;
	
	UFUNCTION()
	void OnRep_bIsVisible();
	
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY()
	FTimerHandle ResetTimer;

	void ResetPickUp();
};
