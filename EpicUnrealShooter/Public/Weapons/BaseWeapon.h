// Copyright Saber Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"

#include "BaseWeapon.generated.h"

class ABaseWeapon;
class APlayerCharacter;

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Pistol = 0,
	Rifle,
	Shotgun,
	GrenadeLauncher,
	SniperRifle,
	RocketLauncher,
	Grenade,
	Knife
};

UCLASS()
class EPICUNREALSHOOTER_API ABaseWeapon : public AActor
{
	GENERATED_BODY()

public:
	ABaseWeapon();
	
	UFUNCTION()
	APlayerCharacter* GetWeaponOwner() const;

	UFUNCTION()
	EWeaponType GetWeaponType() const;
	
	UFUNCTION()
	void SetWeaponOwner(APlayerCharacter* NewOwner);

	UFUNCTION()
	virtual void StartFire();
	UFUNCTION()
	virtual void StopFire();

	virtual bool UseWeapon();
	
	virtual bool CanUseWeapon() const;

	UFUNCTION(BlueprintNativeEvent)
	void SecondActionStart();
	UFUNCTION(BlueprintNativeEvent)
	void SecondActionStop();
	
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* EquipAnimMontage;

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* ReloadAnimMontage;

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* FireAnimMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystemComponent* FireEffectComponent;
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon")
	USoundCue* UseWeaponSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon")
	EWeaponType WeaponType;
	
	UPROPERTY(ReplicatedUsing=OnRep_CurrentAmmo, EditAnywhere, BlueprintReadOnly, Category="Weapon")
	int32 CurrentAmmo = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon")
	float Damage = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon")
	float UseRate = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon")
	bool bAutoFire = true;

	bool bFireActive = false;
	
	UFUNCTION()
	void OnRep_CurrentAmmo();
	
	UFUNCTION()
	void StopRateDelay();

	UFUNCTION()
    void OnRep_WeaponOwner();
	
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_WeaponOwner)
	APlayerCharacter* WeaponOwner;
	
	void StopUseWeapon();
	
	UPROPERTY(Replicated)
	bool bUsingWeapon = false;
	
	UPROPERTY(Replicated)
	FTimerHandle CoolDownTimer;
};
