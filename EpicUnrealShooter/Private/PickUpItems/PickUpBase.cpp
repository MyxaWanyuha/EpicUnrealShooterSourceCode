// Copyright Saber Studio. All Rights Reserved.


#include "PickUpItems/PickUpBase.h"


#include "Engine/DemoNetDriver.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

APickUpBase::APickUpBase()
{
	SetReplicates(true);
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollsionComponent"));
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &APickUpBase::OnOverlapBegin);
	EffectComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EffectComponent"));
	EffectComponent->SetupAttachment(CollisionComponent);
	EffectComponent->SetRelativeLocation(FVector(0.f, 0.f, -30.f));
}

void APickUpBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APickUpBase, bIsVisible);
}

void APickUpBase::OnRep_bIsVisible()
{
	if(bIsVisible == false)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickUpSound, GetActorLocation(), GetActorRotation());
	}
}

void APickUpBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void APickUpBase::ResetPickUp()
{
	bIsVisible = true;
	SetHidden(false);
}

