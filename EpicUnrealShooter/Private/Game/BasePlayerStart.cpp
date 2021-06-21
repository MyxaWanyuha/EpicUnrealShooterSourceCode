// Copyright Saber Studio. All Rights Reserved.


#include "Game/BasePlayerStart.h"

#include "Components/CapsuleComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogBasePlayerStart, All, All);

void ABasePlayerStart::BeginPlay()
{
	Super::BeginPlay();

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABasePlayerStart::OnCapsuleBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ABasePlayerStart::OnCapsuleEndOverlap);
}

APlayerCharacter* ABasePlayerStart::SpawnPlayer(APlayerController* Controller)
{
	UE_LOG(LogBasePlayerStart, Display, TEXT("ABasePlayerStart::SpawnPlayer %p"), Controller);
	if(!Controller) return nullptr;
	if(auto World = GetWorld())
	{
		UE_LOG(LogBasePlayerStart, Display, TEXT("World %p"), World);
		CharacterInside = World->SpawnActor<APlayerCharacter>(PlayerClass, GetTransform());
		UE_LOG(LogBasePlayerStart, Display, TEXT("CharacterInside %p"), CharacterInside);
		Controller->Possess(CharacterInside);
	}

	return CharacterInside;
}

void ABasePlayerStart::OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CharacterInside = Cast<APlayerCharacter>(OtherActor);
}

void ABasePlayerStart::OnCapsuleEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor == CharacterInside)
	{
		CharacterInside = nullptr;
	}
}
