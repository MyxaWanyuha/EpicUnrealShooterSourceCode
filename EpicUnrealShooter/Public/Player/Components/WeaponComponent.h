// Copyright Saber Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapons/BaseProjectile.h"
#include "Weapons/BaseWeapon.h"
#include "WeaponComponent.generated.h"

class APlayerCharacter;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EPICUNREALSHOOTER_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponComponent();
	UPROPERTY(ReplicatedUsing=OnRep_CurrentWeapon, BlueprintReadOnly, Category="Weapon")
	ABaseWeapon* CurrentWeapon;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Weapon")
	TArray<TSubclassOf<ABaseWeapon>> WeaponClasses;
	
	UFUNCTION(BlueprintCallable)
	ABaseWeapon* CreateWeapon(TSubclassOf<ABaseWeapon> Class);

	UFUNCTION(BlueprintCallable)
    void AddWeapon(ABaseWeapon* Weapon);

	bool AddAmmo();
	
	UFUNCTION()
	ABaseWeapon* SelectWeapon(ABaseWeapon* Weapon, EWeaponType Type); 

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ABaseWeapon* GetCurrentWeapon();

	bool CanFire() const;
	bool CanEquip() const;
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Weapon")
	ABaseWeapon* Pistol;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Weapon")
	ABaseWeapon* Rifle;
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Weapon")
	ABaseWeapon* Shotgun;
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Weapon")
	ABaseWeapon* GrenadeLauncher;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Weapon")
	ABaseWeapon* SniperRifle;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Weapon")
	ABaseWeapon* RocketLauncher;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Weapon")
	ABaseWeapon* Grenade;
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Weapon")
	ABaseWeapon* Knife;
protected:
	
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_CurrentWeapon();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	FName WeaponSocketName = "WeaponSocket";
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	APlayerCharacter* Owner;
private:
	UPROPERTY()
	ABaseWeapon* ClientWeaponAttached;

	bool bEquipAnimInProgress = false;
	bool bReloadAnimInProgress = false;

	FTimerHandle AnimTimerHandle;
	float PlayAnimMontage(UAnimMontage* Animation) const;
	UFUNCTION()
	void OnEquipFinished();
};
