// Copyright Saber Studio. All Rights Reserved.


#include "Weapons/FireHole.h"

AFireHole::AFireHole()
{
	PrimaryActorTick.bCanEverTick = false;
	SetReplicates(true);
}

void AFireHole::BeginPlay()
{
	Super::BeginPlay();
	
}
