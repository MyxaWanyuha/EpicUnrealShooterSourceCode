// Copyright Saber Studio. All Rights Reserved.


#include "Game/BaseGameState.h"


#include "Engine/DemoNetDriver.h"
#include "Game/BaseGameMode.h"
#include "Game/BasePlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SpectatorPawn.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseGameState, All, All);

void ABaseGameState::BeginPlay()
{
	Super::BeginPlay();
	
	if(HasAuthority())
	{
		if(auto World = GetWorld())
		{
			World->GetTimerManager().SetTimer(TickSec, this, &ABaseGameState::TickPerSec, 1.f, true);
			auto GM = Cast<ABaseGameMode>(World->GetAuthGameMode());
			if(GM)
			{
				GM->OnRoundEnded.AddDynamic(this, &ABaseGameState::OnRoundEnded);
			}
		}
	}

	GameMode = Cast<ABaseGameMode>(GetDefaultGameMode());
	check(GameMode);
}

void ABaseGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseGameState, RoundInProgress);
	DOREPLIFETIME(ABaseGameState, CurrentMinute);
	DOREPLIFETIME(ABaseGameState, CurrentSecond);
}

int32 ABaseGameState::GetCurrentSecond() const
{
	return CurrentSecond;
}

int32 ABaseGameState::GetCurrentMinute() const
{
	return CurrentMinute;
}

void ABaseGameState::OnRep_RoundInProgress()
{
	if(EGameState::Game == RoundInProgress)
	{
		OnRoundStarted.Broadcast();
	}
}

void ABaseGameState::OnRoundEnded(const FString& Reason)
{
	RoundInProgress = EGameState::RoundEnd;
	if(auto GM = Cast<ABaseGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->SwitchMap();
	}
}

void ABaseGameState::StopRound(const FString& Reason)
{
	if(!HasAuthority()) return;
	for(auto PS : PlayerArray)
	{
		if(auto const Controller = Cast<ABasePlayerController>(PS->GetOwner()))
		{
			auto World = GetWorld();
			if(!World) return;
			
			auto Spectator = World->SpawnActor<ASpectatorPawn>(GameMode->SpectatorClass, Controller->GetPawn()->GetTransform());
			Controller->UnPossess();
			Controller->Possess(Cast<APawn>(Spectator));
		}
	}
	
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(this, GameMode->DefaultPawnClass, OutActors);
	for(auto CharPlayer : OutActors)
	{
		CharPlayer->Destroy();
	}
	RoundInProgress = EGameState::RoundEnd;
	OnRoundEnded(Reason);
}

void ABaseGameState::RoundTimeTick()
{
	const FTimespan oneSec(0, 0, 0, 1);
	RoundTime += oneSec;
	CurrentSecond = RoundTime.GetSeconds();
	CurrentMinute = RoundTime.GetMinutes();

	if(RoundInProgress == EGameState::Game && RoundTime >= GameMode->GetRoundTimeLimit())
	{
		StopRound("time limit");
	}
}

bool ABaseGameState::EnoughPlayersToStartRound()
{
	return PlayerArray.Num() >= PlayersCountToStartRound;
}

void ABaseGameState::TickPerSec()
{
	if(HasAuthority())
	{
		if(EGameState::Game == RoundInProgress)
		{
			RoundTimeTick();
		}
		else if(EGameState::PreStart == RoundInProgress)
		{
			if(EnoughPlayersToStartRound())
			{
				if(--RoundStartTime <= 0)
				{
					GameMode->StartRound();
					RoundInProgress = EGameState::Game;
				}
			}
			else
			{
				RoundStartTime = RoundStartTimeDefault;
			}
		}
	}
}
