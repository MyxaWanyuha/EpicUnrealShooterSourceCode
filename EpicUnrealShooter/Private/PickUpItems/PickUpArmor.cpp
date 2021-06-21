// Copyright Saber Studio. All Rights Reserved.


#include "PickUpItems/PickUpArmor.h"

#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

void APickUpArmor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!bIsVisible || !HasAuthority()) return;
	if(auto Character = Cast<APlayerCharacter>(OtherActor))
	{
		if(HasAuthority() && Character->AddArmor(CountToAdd))
		{
			SetHidden(true);
			GetWorldTimerManager().SetTimer(ResetTimer, this, &APickUpBase::ResetPickUp, ResetTime);
			bIsVisible = false;
		}
	}
}
