// Copyright Saber Studio. All Rights Reserved.


#include "PickUpItems/PickUpAmmo.h"

#include "PlayerCharacter.h"
#include "Particles/ParticleSystemComponent.h"

void APickUpAmmo::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!bIsVisible || !HasAuthority()) return;
	if(auto Character = Cast<APlayerCharacter>(OtherActor))
	{
		if(HasAuthority() && Character->AddAmmo())
		{
			SetHidden(true);
			GetWorldTimerManager().SetTimer(ResetTimer, this, &APickUpBase::ResetPickUp, ResetTime);
			bIsVisible = false;
		}
	}
}
