// Copyright Saber Studio. All Rights Reserved.


#include "Player/Components/WeaponComponent.h"

#include "Camera/CameraComponent.h"
#include "Player/PlayerCharacter.h"
#include "Engine/DemoNetDriver.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Weapons/FireWeapon.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeaponComponent, All, All);

UWeaponComponent::UWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

ABaseWeapon* UWeaponComponent::CreateWeapon(TSubclassOf<ABaseWeapon> Class)
{
	check(Owner);

	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	ActorSpawnParams.Owner = GetOwner();

	auto World = GetWorld();
	if(!World) return nullptr;
	auto LWeapon = World->SpawnActor<ABaseWeapon>(Class, FTransform::Identity, ActorSpawnParams);
	if(LWeapon)
	{
		LWeapon->SetWeaponOwner(Owner);
	}
	return LWeapon;
}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<APlayerCharacter>(GetOwner());
	check(Owner);
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UWeaponComponent, Owner);
	DOREPLIFETIME(UWeaponComponent, CurrentWeapon);
	DOREPLIFETIME_CONDITION(UWeaponComponent, Pistol,			COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UWeaponComponent, Rifle,			COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UWeaponComponent, Shotgun,			COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UWeaponComponent, GrenadeLauncher,	COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UWeaponComponent, SniperRifle,		COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UWeaponComponent, RocketLauncher,	COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UWeaponComponent, Grenade,			COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UWeaponComponent, Knife,			COND_OwnerOnly);
}

void UWeaponComponent::OnRep_CurrentWeapon()
{
	if(!UKismetSystemLibrary::IsDedicatedServer(this))
	{
		if(IsValid(CurrentWeapon))
		{
			const auto AnimTime = PlayAnimMontage(CurrentWeapon->EquipAnimMontage);
			GetWorld()->GetTimerManager().SetTimer(AnimTimerHandle, this, &UWeaponComponent::OnEquipFinished, AnimTime, false);
			bEquipAnimInProgress = true;
			const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true);
			CurrentWeapon->SetActorHiddenInGame(false);
			if(IsValid(ClientWeaponAttached))
			{
				ClientWeaponAttached->AttachToActor(Owner, AttachmentTransformRules);
				ClientWeaponAttached->SetActorHiddenInGame(true);				
			}
			check(CurrentWeapon);
			CurrentWeapon->AttachToComponent(Owner->GetVisibleMesh(), AttachmentTransformRules, WeaponSocketName);
			ClientWeaponAttached = CurrentWeapon;
		}
		else
		{
			ClientWeaponAttached = nullptr;
		}
	}
}

bool UWeaponComponent::CanFire() const
{
	return CurrentWeapon && !bEquipAnimInProgress && !bReloadAnimInProgress;
}

bool UWeaponComponent::CanEquip() const
{
	return !bEquipAnimInProgress && !bReloadAnimInProgress;
}

float UWeaponComponent::PlayAnimMontage(UAnimMontage* Animation) const
{
	if(!Owner) return 0.f;
	return Owner->PlayAnimMontage(Animation);
}

void UWeaponComponent::OnEquipFinished()
{
	GetWorld()->GetTimerManager().ClearTimer(AnimTimerHandle);
	bEquipAnimInProgress = false;
}

ABaseWeapon* UWeaponComponent::SelectWeapon(ABaseWeapon* Weapon, EWeaponType Type)
{
	UE_LOG(LogWeaponComponent, Display, TEXT("UWeaponComponent::SelectWeapon %p"), Weapon)
	ABaseWeapon* w = nullptr;
	switch(Type)
	{
	case EWeaponType::Pistol:
		w = Pistol;
		break;
	case EWeaponType::Rifle:
		w = Rifle;
		break;
	case EWeaponType::Shotgun:
		w = Shotgun;
		break;
	case EWeaponType::GrenadeLauncher:
		w = GrenadeLauncher;
		break;
	case EWeaponType::SniperRifle:
		w = SniperRifle;
		break;
	case EWeaponType::RocketLauncher:
		w = RocketLauncher;
		break;
	case EWeaponType::Grenade:
		w = Grenade;
		break;
	case EWeaponType::Knife:
		w = Knife;
		break;
	default:
		break;
	}

	if(IsValid(w))
	{
		if(auto FireWeapon = Cast<AFireWeapon>(CurrentWeapon))
		{
		    if(FireWeapon->IsReloading())
			{
				FireWeapon->StopReload();
			}
		}

		CurrentWeapon = w;
		if(auto FireWeapon = Cast<AFireWeapon>(CurrentWeapon))
		{
			if(FireWeapon->IsNeedReload())
			{
				FireWeapon->ReloadWeapon();
			}
		}
		return CurrentWeapon;
	}
	return nullptr;
}

ABaseWeapon* UWeaponComponent::GetCurrentWeapon()
{
	return CurrentWeapon;
}

void UWeaponComponent::AddWeapon(ABaseWeapon* Weapon)
{
	if(!Weapon) return;
	switch(Weapon->GetWeaponType())
	{
	case EWeaponType::Pistol:
			if(!Pistol)
				Pistol = Weapon;
		break;
	case EWeaponType::Rifle:
		if(!Rifle)
			Rifle = Weapon;
		break;
	case EWeaponType::Shotgun:
		if(!Shotgun)
			Shotgun = Weapon;
		break;
	case EWeaponType::GrenadeLauncher:
		if(!GrenadeLauncher)
			GrenadeLauncher = Weapon;
		break;
	case EWeaponType::SniperRifle:
		if(!SniperRifle)
			SniperRifle = Weapon;
		break;
	case EWeaponType::RocketLauncher:
		if(!RocketLauncher)
			RocketLauncher = Weapon;
		break;
	case EWeaponType::Grenade:
		if(!Grenade)
			Grenade = Weapon;
		break;
	case EWeaponType::Knife:
		if(!Knife)
			Knife = Weapon;
		break;
	default:
		break;
	}

	if(!IsValid(CurrentWeapon))
	{
		CurrentWeapon = Weapon;
	}
}

bool UWeaponComponent::AddAmmo()
{
	int res = 0;
	if(auto p = Cast<AFireWeapon>(Pistol))
		res += p->AddAmmo();
	if(auto p = Cast<AFireWeapon>(Rifle))
		res += p->AddAmmo();
	if(auto p = Cast<AFireWeapon>(Shotgun))
		res += p->AddAmmo();
	if(auto p = Cast<AFireWeapon>(GrenadeLauncher))
		res += p->AddAmmo();
	if(auto p = Cast<AFireWeapon>(SniperRifle))
		res += p->AddAmmo();
	if(auto p = Cast<AFireWeapon>(RocketLauncher))
		res += p->AddAmmo();
	if(auto p = Cast<AFireWeapon>(Grenade))
		res += p->AddAmmo();

	return res != 0;
}
