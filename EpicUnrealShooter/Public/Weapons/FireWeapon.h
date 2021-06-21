// Copyright Saber Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BaseWeapon.h"
#include "FireWeapon.generated.h"

class ABaseProjectile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReloadStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReloadFinished);

UCLASS()
class EPICUNREALSHOOTER_API AFireWeapon : public ABaseWeapon
{
	GENERATED_BODY()

public:
	AFireWeapon();
	FOnReloadStarted OnReloadStarted;
	FOnReloadFinished OnReloadFinished;
	
	virtual void StartFire() override;
    virtual void StopFire() override;
	
	UFUNCTION()
	bool CanReloadWeapon() const;
	
	UFUNCTION()
	void ReloadWeapon();
	
	virtual bool CanUseWeapon() const override;

	UFUNCTION()
	bool IsNeedReload() const { return CurrentAmmo < 1; }

	UFUNCTION()
	bool IsReloading() const { return bIsReloading; }
	
	UFUNCTION()
    void StopReload();

	bool AddAmmo();
	
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing=OnRep_bIsReloading, BlueprintReadOnly, Category="Weapon|Reload")
	bool bIsReloading = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon|Reload")
	float ReloadTime = 3.f;
	
	int32 AmmoToReload = 0;

	FTimerHandle ReloadTimer;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="Weapon")
	int32 TotalAmmo = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	int32 MaxTotalAmmo = 10.f;
	
	UPROPERTY(EditAnywhere, Category="Weapon")
	TSubclassOf<ABaseProjectile> FireProjectile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	float SpreadAngle = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	FRandomStream RandomStream;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Bullet")
	float ProjectileForwardVectorMultiplier = 50.f;
	
	UFUNCTION()
	void FinishReload();
	
	UFUNCTION()
	void OnRep_bIsReloading();

	UFUNCTION()
	void StartReload();
	
	virtual bool UseWeapon() override;

	UFUNCTION(BlueprintCallable)
	void SpawnShootProjectile();

	UFUNCTION()
	FVector GetShootDirection() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon|Recoil")
	bool bIsCanRecoil = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon|Recoil")
	float RecoilPitchMin = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon|Recoil")
	float RecoilPitchMax = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon|Recoil")
	float RecoilYawMin = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon|Recoil")
	float RecoilYawMax = 0.f;

private:

	UPROPERTY(ReplicatedUsing=OnRep_bRecoil)
	bool bRecoil = false;

	UFUNCTION()
    void OnRep_bRecoil();
	
	int32 DefaultCurrentAmmo = 0.f;
};
