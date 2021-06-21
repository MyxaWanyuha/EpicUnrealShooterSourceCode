// Copyright Saber Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "Components/ActorComponent.h"

#include "ConditionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharDead);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnClientDamageReceived, float, Damage, FVector, DamageDirection);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EPICUNREALSHOOTER_API UConditionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UConditionComponent();

	UFUNCTION(BlueprintCallable)
	float GetHealth() const;
	
	UFUNCTION(BlueprintCallable)
	float GetArmor() const;

	UFUNCTION(BlueprintCallable)
	bool IsDead() const;
	
	UPROPERTY(BlueprintAssignable)
	FOnCharDead OnCharDead;

	bool AddHealth(float Count);
	bool AddArmor(float Count);
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="Health")
	float MaxHealth = 200.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="Health")
	float InitHealth = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="Armor")
	float MaxArmor = 150.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="Armor")
	float InitArmor = 25.f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Health")
	bool bIsDead = false;
	
	UPROPERTY(BlueprintReadWrite, Category="Owner")
	APlayerCharacter* Owner;

	UPROPERTY(BlueprintAssignable)
	FOnClientDamageReceived OnClientDamageReceived;

private:

	UFUNCTION(Client, Unreliable)
	void NotifyDamage(float Damage, FVector DamageVector);
	
	UPROPERTY(Replicated)
	float Health;
	
	UPROPERTY(Replicated)
	float Armor;
	
	void ApplyDamage(float Damage);
	
	bool CanBeDamaged(UObject* Instigator) const;

	UFUNCTION()
	void OnPlayerTakeAnyDamage(AActor* DamagedActor, float Damage,
		const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
};
