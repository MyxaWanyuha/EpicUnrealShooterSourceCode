// Copyright Saber Studio. All Rights Reserved.


#include "Weapons/BaseWeapon.h"


#include "PlayerCharacter.h"
#include "WeaponComponent.h"
#include "Engine/DemoNetDriver.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseWeapon, All, All);

ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	SetReplicates(true);
	NetUpdateFrequency = 10;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	FireEffectComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireEffectComponent"));
	FireEffectComponent->SetupAttachment(RootComponent);
	
	SetActorEnableCollision(false);
}

APlayerCharacter* ABaseWeapon::GetWeaponOwner() const
{
	return WeaponOwner;
}

EWeaponType ABaseWeapon::GetWeaponType() const
{
	return WeaponType;
}

void ABaseWeapon::SetWeaponOwner(APlayerCharacter* NewOwner)
{
	if(!WeaponOwner)
	{
		WeaponOwner = NewOwner;
		SetOwner(WeaponOwner);
	}
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

}

void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseWeapon, CurrentAmmo);
	DOREPLIFETIME(ABaseWeapon, bUsingWeapon);
	DOREPLIFETIME(ABaseWeapon, WeaponOwner);
	DOREPLIFETIME(ABaseWeapon, CoolDownTimer);
}

void ABaseWeapon::StartFire(){}

void ABaseWeapon::StopFire(){}

void ABaseWeapon::OnRep_CurrentAmmo()
{
	if(bUsingWeapon)
	{
		WeaponOwner->PlayAnimMontage(FireAnimMontage);
		UGameplayStatics::PlaySoundAtLocation(this, UseWeaponSound, GetActorLocation());
		FireEffectComponent->Activate(true);
	}
}

void ABaseWeapon::StopRateDelay()
{
	GetWorld()->GetTimerManager().ClearTimer(CoolDownTimer);
	if(bAutoFire && bFireActive)
	{
		UseWeapon();
	}
}

bool ABaseWeapon::CanUseWeapon() const
{
	return !CoolDownTimer.IsValid() && CurrentAmmo > 0;
}

void ABaseWeapon::SecondActionStart_Implementation()
{
	
}

void ABaseWeapon::SecondActionStop_Implementation()
{
	
}

bool ABaseWeapon::UseWeapon()
{
	if(CanUseWeapon())
	{
		--CurrentAmmo;
		bUsingWeapon = true;
		GetWorldTimerManager().SetTimer(CoolDownTimer, this, &ABaseWeapon::StopRateDelay, UseRate);

		if(bAutoFire)
		{
			bFireActive = true;
		}
		return true;
	}
	bFireActive = false;
	bUsingWeapon = false;
	return false;
}

void ABaseWeapon::StopUseWeapon()
{
	bFireActive = false;
	bUsingWeapon = false;
}

void ABaseWeapon::OnRep_WeaponOwner()
{
	if(WeaponOwner)
	{
		const FAttachmentTransformRules AttachmentTransformRules(
	EAttachmentRule::SnapToTarget,
	EAttachmentRule::SnapToTarget,
	EAttachmentRule::KeepRelative,
	true);
		SetInstigator(WeaponOwner);
		SetOwner(GetInstigatorController());
		SetActorEnableCollision(false);
		
		AttachToActor(WeaponOwner, AttachmentTransformRules);
		if(WeaponOwner->GetWeaponComponent()->CurrentWeapon != this)
		{
			SetActorHiddenInGame(true);
		}
		else
		{
			AttachToComponent(WeaponOwner->GetVisibleMesh(), AttachmentTransformRules, FName{"Skt_Weapon"});
		}
	}
	else
	{
		DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		SetActorHiddenInGame(true);
	}
}
