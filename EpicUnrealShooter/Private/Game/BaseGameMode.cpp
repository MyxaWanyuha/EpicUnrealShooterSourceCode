// Copyright Saber Studio. All Rights Reserved.


#include "Game/BaseGameMode.h"

#include "Game/BasePlayerController.h"
#include "PlayerCharacter.h"
#include "Game/BaseGameState.h"
#include "Game/BasePlayerStart.h"
#include "Game/BasePlayerState.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SpectatorPawn.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseGameMode, All, All);

void ABaseGameMode::BeginPlay()
{
	Super::BeginPlay();
	bUseSeamlessTravel = true;
}

void ABaseGameMode::StartRound() const
{
}

const FTimespan& ABaseGameMode::GetRoundTimeLimit() const
{
	return RoundTimeLimit;
}

void ABaseGameMode::TimeLimitReached() const
{
}

void ABaseGameMode::CharacterDead(APlayerCharacter* DeadCharacter, APlayerController* DeathInstigator,
	AActor* DeathCausedBy)
{
	OnCharacterDead.Broadcast(DeadCharacter, DeathInstigator, DeathCausedBy);
	auto PlayerState = Cast<ABasePlayerState>(DeadCharacter->GetPlayerState());
	const auto PlayerCharacter = Cast<APlayerCharacter>(DeathInstigator->GetPawn());
	if(PlayerState && PlayerCharacter)
	{
		PlayerState->OwnerCharDead(PlayerCharacter, DeathCausedBy);
	}
	auto DeadPlayerController = Cast<ABasePlayerController>(DeadCharacter->GetController());
	if(!DeadPlayerController) return;
	
	if(bAutoRespawn)
	{
		DeadPlayerController->LaunchRespawnTimer(RespawnTime);
	}

	auto World = GetWorld();
	if(!World) return;
	auto Spectator = World->SpawnActor<ASpectatorPawn>(SpectatorClass, DeadPlayerController->GetPawn()->GetTransform());
	DeadPlayerController->UnPossess();
	DeadPlayerController->Possess(Cast<APawn>(Spectator));
}

void ABaseGameMode::SpawnPlayer(APlayerController* Controller)
{
	if(IsValid(Controller->GetPawn()))
	{
		Controller->GetPawn()->Destroy();
	}
	
	auto FreePoints = GetFreeSpawnPoints();
	
	APlayerCharacter* Char = nullptr;
	for(auto i = 0; Char == nullptr && i < FreePoints.Num(); ++i)
	{
		UE_LOG(LogBaseGameMode, Display, TEXT("TrySpawn %i"), i)
		Char = FreePoints[i]->SpawnPlayer(Controller);
	}
	Cast<ABasePlayerState>(Controller->PlayerState)->bIsAlive = true;
}

TArray<ABasePlayerStart*> ABaseGameMode::GetFreeSpawnPoints()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABasePlayerStart::StaticClass(), FoundActors);

	TArray<ABasePlayerStart*> FreePoints;
	for(auto Actor : FoundActors)
	{
		auto Spawn = Cast<ABasePlayerStart>(Actor);
		if(Spawn->IsFree())
		{
			FreePoints.Add(Spawn);
		}
	}
	return FreePoints;
}

void ABaseGameMode::UpdateDeathPoints(APlayerController* Character, APlayerController* DeathInstigator) const
{
	Cast<ABasePlayerState>(Character->PlayerState)->AddDeath();
	Cast<ABasePlayerState>(DeathInstigator->PlayerState)->AddKill();
}

void ABaseGameMode::OnPlayerConnected(APlayerState* PC)
{
	if(bAllowJoinInProgress && GetGameState<ABaseGameState>()->GetRoundInProgress() == EGameState::Game)
	{
		if(auto PCOwner = Cast<ABasePlayerController>(PC->GetOwner()))
		{
			//TODO
			//PCOwner->SpawnPlayer();
		}
		else
		{
			UE_LOG(LogBaseGameMode, Warning, TEXT("Failed to spawn player on connect. NO Controller!!"));
		}
	}
}

void ABaseGameMode::SwitchMap()
{
	UE_LOG(LogBaseGameMode, Display, TEXT("Switch map"));
	FString mapname = GetWorld()->GetMapName() == FString(TEXT("M_Map1")) ? TEXT("M_Test") : TEXT("M_Map1");
	UE_LOG(LogBaseGameMode, Display, TEXT("%s"), *mapname);
	ProcessServerTravel(mapname);
}
