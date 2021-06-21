// Copyright Saber Studio. All Rights Reserved.


#include "Weapons/FireWeapon.h"
#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Engine/DemoNetDriver.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Weapons/BaseProjectile.h"

DEFINE_LOG_CATEGORY_STATIC(LogFireWeapon, All, All);

AFireWeapon::AFireWeapon()
{
	SetReplicates(true);
}

void AFireWeapon::StartFire()
{
	if(CanUseWeapon())
	{
		WeaponOwner->AddControllerPitchInput(FMath::RandRange(-0.1f, -0.5f));
		WeaponOwner->AddControllerYawInput(FMath::RandRange(-0.3f, 0.3f));
		UseWeapon();
	}
}

void AFireWeapon::StopFire()
{
	StopUseWeapon();
}

void AFireWeapon::BeginPlay()
{
	ABaseWeapon::BeginPlay();
	DefaultCurrentAmmo = CurrentAmmo;
}

void AFireWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AFireWeapon, TotalAmmo,	 COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AFireWeapon, bRecoil,	 COND_OwnerOnly);
	DOREPLIFETIME(AFireWeapon, bIsReloading);
}

void AFireWeapon::FinishReload()
{
	bIsReloading = false;
	CurrentAmmo += AmmoToReload;
	TotalAmmo -= AmmoToReload;
	GetWorldTimerManager().ClearTimer(ReloadTimer);
	OnReloadFinished.Broadcast();
}

void AFireWeapon::ReloadWeapon()
{
	if(!bIsReloading)
	{
		bIsReloading = true;
		StartReload();
	}
}

void AFireWeapon::StartReload()
{
	if(bIsReloading && CanReloadWeapon())
	{
		ReloadTime = WeaponOwner->PlayAnimMontage(ReloadAnimMontage);
		GetWorldTimerManager().SetTimer(ReloadTimer, this, &AFireWeapon::FinishReload, ReloadTime, false);
		AmmoToReload = FMath::Min(DefaultCurrentAmmo - CurrentAmmo, TotalAmmo);
		OnReloadStarted.Broadcast();
	}
	else
	{
		bIsReloading = false;
	}
}

void AFireWeapon::StopReload()
{
	bIsReloading = false;
	WeaponOwner->StopAnimMontage();
	GetWorldTimerManager().ClearTimer(ReloadTimer);
}

bool AFireWeapon::AddAmmo()
{
	if(TotalAmmo == MaxTotalAmmo) return false;
	TotalAmmo = MaxTotalAmmo;
	return true;
}

void AFireWeapon::OnRep_bIsReloading()
{
	if(bIsReloading)
	{
		WeaponOwner->PlayAnimMontage(ReloadAnimMontage);
	}
}

bool AFireWeapon::CanReloadWeapon() const
{
	return  TotalAmmo > 0
	&& DefaultCurrentAmmo != CurrentAmmo
	&& !ReloadTimer.IsValid();
}

bool AFireWeapon::UseWeapon()
{
	if(ABaseWeapon::UseWeapon())
	{
		SpawnShootProjectile();
		if(CurrentAmmo <= 0 && TotalAmmo > 0)
		{
			ReloadWeapon();
		}
		bRecoil = bIsCanRecoil && !bRecoil;
		return true;
	}
	return false;
}

void AFireWeapon::SpawnShootProjectile()
{
	if (auto World = GetWorld())
	{
		const FVector CameraLocation(WeaponOwner->GetCamera()->GetComponentLocation());
		const FVector OwnerForwardVector(WeaponOwner->GetController()->AActor::GetActorForwardVector() * ProjectileForwardVectorMultiplier);
		const FVector SpawnLocation(CameraLocation + OwnerForwardVector);
		const FRotator Direction(GetShootDirection().ToOrientationRotator());
		const FTransform SpawnTransform(FTransform(Direction, SpawnLocation));
		
		auto Bullet = World->SpawnActorDeferred<ABaseProjectile>
        (FireProjectile,
         SpawnTransform,
         this,
         GetInstigator(),
         ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		if (Bullet)
		{
			Bullet->SetOwner(this);
			Bullet->Damage = Damage;
			Bullet->Weapon = this;
			Bullet->FinishSpawning(SpawnTransform);
		}
	}
}

FVector AFireWeapon::GetShootDirection() const
{
	check(WeaponOwner);

	const auto VerticalAngle = WeaponOwner->GetControlRotation().Pitch * -1.f;
	const FVector RotatedVector1 = WeaponOwner->GetActorForwardVector().RotateAngleAxis(
        VerticalAngle, WeaponOwner->GetActorRightVector().GetSafeNormal());

	const FRotator RV1Rotator = UKismetMathLibrary::Conv_VectorToRotator(RotatedVector1);

	const float AngleToSpread1 = UKismetMathLibrary::RandomFloatInRangeFromStream(
        SpreadAngle * -1.f, SpreadAngle, RandomStream);
	const FVector Rot1 = UKismetMathLibrary::GetRightVector(RV1Rotator);
	const FVector RotatedVector2 = UKismetMathLibrary::RotateAngleAxis(RotatedVector1, AngleToSpread1, Rot1);

	const float AngleToSpread2 = UKismetMathLibrary::RandomFloatInRangeFromStream(
        SpreadAngle * -1.f, SpreadAngle, RandomStream);
	const FVector Rot2 = UKismetMathLibrary::GetUpVector(RV1Rotator);

	return RotatedVector2.RotateAngleAxis(AngleToSpread2, Rot2.GetSafeNormal());
}

void AFireWeapon::OnRep_bRecoil()
{
	WeaponOwner->AddControllerPitchInput(FMath::RandRange(RecoilPitchMin, RecoilPitchMax));
	WeaponOwner->AddControllerYawInput(FMath::RandRange(RecoilYawMin, RecoilYawMax));
}

bool AFireWeapon::CanUseWeapon() const
{
	return !bIsReloading && ABaseWeapon::CanUseWeapon();
}
