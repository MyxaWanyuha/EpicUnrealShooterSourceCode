// Copyright Saber Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PlayerCharacter.h"
#include "GameFramework/GameModeBase.h"
#include "BaseGameMode.generated.h"

class ABasePlayerStart;
class ABasePlayerController;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoundEnded, const FString&, Reason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCharacterDeadGM, APlayerCharacter*, DeadCharacter, APlayerController*, DeathInstigator, AActor*, DeathCausedBy);

UCLASS()
class EPICUNREALSHOOTER_API ABaseGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	void StartRound() const;
	const FTimespan& GetRoundTimeLimit() const;
	void TimeLimitReached() const;
	void CharacterDead(APlayerCharacter* DeadCharacter, APlayerController* DeathInstigator, AActor* DeathCausedBy);
	void SpawnPlayer(APlayerController* Controller);
	FOnRoundEnded OnRoundEnded;
	FOnCharacterDeadGM OnCharacterDead;

	void SwitchMap();
	
	void UpdateDeathPoints(APlayerController* Character, APlayerController* DeathInstigator) const;
	
protected:

	TArray<ABasePlayerStart*> GetFreeSpawnPoints();

	void OnPlayerConnected(APlayerState* PC);
	
	UPROPERTY(EditAnywhere, Category="Limit")
	FTimespan RoundTimeLimit;
	
	UPROPERTY(EditAnywhere, Category="Limit")
	bool bLimitByTime = true;

	UPROPERTY(EditAnywhere, Category="Respawn")
	bool bAutoRespawn = true;

	UPROPERTY(EditAnywhere, Category="Respawn")
	float RespawnTime = 2.0f;
	
	UPROPERTY(EditAnywhere, Category="Connect")
	bool bAllowJoinInProgress = true;
};
