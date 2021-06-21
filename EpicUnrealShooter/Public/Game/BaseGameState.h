// Copyright Saber Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameStateBase.h"
#include "Game/BaseGameMode.h"
#include "BaseGameState.generated.h"

class APlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRoundStarted);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimeEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerConnected, APlayerState*, PC);

UENUM()
enum class EGameState : uint8
{
	PreStart,
	Game,
	RoundEnd
};

UCLASS()
class EPICUNREALSHOOTER_API ABaseGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > &OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentMinute() const;

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentSecond() const;
	EGameState GetRoundInProgress() const { return RoundInProgress; }
	
protected:
	UPROPERTY(ReplicatedUsing=OnRep_RoundInProgress)
	EGameState RoundInProgress = EGameState::PreStart;
	
	UFUNCTION()
	void StopRound(const FString& Reason);

	UFUNCTION()
	void OnRep_RoundInProgress();
	
	UFUNCTION()
	void OnRoundEnded(const FString& Reason);
	
	void RoundTimeTick();
	bool EnoughPlayersToStartRound();

	UPROPERTY(EditAnywhere)
	int8 PlayersCountToStartRound = 2;

	UPROPERTY(EditAnywhere)
	int8 RoundStartTime = 1;
	
	FOnRoundStarted OnRoundStarted;
	FOnTimeEnded OnTimeEnded;
private:
	UPROPERTY(Replicated)
	int16 CurrentMinute = 0;

	UPROPERTY(Replicated)
	int8 CurrentSecond = 0;

	UPROPERTY()
	FTimespan RoundTime;
	
	const int8 RoundStartTimeDefault = RoundStartTime;

	UPROPERTY()
	const ABaseGameMode* GameMode;

	UFUNCTION()
	void TickPerSec();
	FTimerHandle TickSec;
};
