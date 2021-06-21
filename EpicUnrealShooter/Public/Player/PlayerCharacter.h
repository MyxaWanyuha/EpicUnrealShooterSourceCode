// Copyright Saber Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "Weapons/BaseWeapon.h"

#include "PlayerCharacter.generated.h"

class UCameraComponent;
class UConditionComponent;
class UWeaponComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDead);

UCLASS()
class EPICUNREALSHOOTER_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()
	
public:
	APlayerCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	UWeaponComponent*		GetWeaponComponent();
	const UCameraComponent*	GetCamera()				const;
	USkeletalMeshComponent*	GetVisibleMesh()		const;

	bool AddHealth(float Count);
	bool AddArmor(float Count);
	bool AddAmmo();
	
	UPROPERTY(BlueprintAssignable)
	FOnCharacterDead OnCharacterDead;

	UFUNCTION(BlueprintCallable)
	void SetFieldOfView(float Value);
	
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void Reload();
	UFUNCTION(Server, Reliable)
    void ServerReload();
	
	void MoveForward(float Value);
	void MoveRight(float Value);

	void StartSecondAction();
	void StopSecondAction();

	void StartFire();
	void StopFire();
	UFUNCTION(Server, Reliable)
	void ServerStartFire();
	UFUNCTION(Server, Reliable)
	void ServerStopFire();
	
	virtual void Jump() override;
	virtual void StopJumping() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* DeathAnimMontage;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	USkeletalMeshComponent* FirstPersonMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	UConditionComponent* ConditionComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	UWeaponComponent* WeaponComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variables")
	float DestroyTimeSeconds = 2.f;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_BodyMaterial)
	UMaterialInstance* BodyMaterial;
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_HeadMaterial)
	UMaterialInstance* HeadMaterial;
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_EyesMaterial)
	UMaterialInstance* EyesMaterial;
	UFUNCTION()
    void OnRep_BodyMaterial();
	UFUNCTION()
	void OnRep_HeadMaterial();
	UFUNCTION()
	void OnRep_EyesMaterial();
	
	UPROPERTY(ReplicatedUsing=OnRep_bDead)
	bool bDead = false;

	UFUNCTION()
	void OnRep_bDead();
	
	UFUNCTION()
    void Death();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void DestroyWeapons();
	
	void SelectPistol();
	void SelectRifle();
	void SelectShotgun();
	void SelectLauncher();
	void SelectSniperRifle();
	void SelectRocketLauncher();
	void SelectGrenade();
	void SelectKnife();
	
	void SelectWeapon(EWeaponType Type);
	UFUNCTION(Server, Unreliable)
	void ServerSelectWeapon(EWeaponType Type);
};
