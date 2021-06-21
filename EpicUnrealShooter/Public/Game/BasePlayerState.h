// Copyright Saber Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PlayerCharacter.h"
#include "GameFramework/PlayerState.h"
#include "BasePlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerAlive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerDeadPS, APlayerCharacter*, Instigator, AActor*, DeathCauser, APlayerState*, PlayerState);

UCLASS()
class EPICUNREALSHOOTER_API ABasePlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void AddDeath();
	void AddKill();

	UFUNCTION(BlueprintCallable)
	int32 GetDeaths() const { return Deaths; }
	UFUNCTION(BlueprintCallable)
	int32 GetKills() const { return Kills; }
	
	FOnPlayerAlive OnPlayerAlive;
	FOnPlayerDeadPS OnPlayerDead;

	UFUNCTION(NetMulticast, Unreliable)
    void OwnerCharDead(APlayerCharacter* LInstigator, AActor* DeathCauser);

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_bIsAlive)
	bool bIsAlive = true;
protected:
	UPROPERTY(Replicated)
	int32 Kills = 0;

	UPROPERTY(Replicated)
	int32 Deaths = 0;

	UFUNCTION()
	void OnRep_bIsAlive();
};
