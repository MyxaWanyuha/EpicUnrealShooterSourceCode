// Copyright Saber Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PlayerCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "BasePlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class EPICUNREALSHOOTER_API ABasePlayerStart : public APlayerStart
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	bool IsFree() const { return CharacterInside == nullptr; }
	APlayerCharacter* SpawnPlayer(APlayerController* Controller);
	
	UPROPERTY()
	APlayerCharacter* CharacterInside = nullptr;

	UPROPERTY(EditAnywhere, Category="SpawnParemeters")
	TSubclassOf<APlayerCharacter> PlayerClass;

	UFUNCTION()
	void OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
 		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnCapsuleEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,int32 OtherBodyIndex);
};
