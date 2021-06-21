// Copyright Saber Studio. All Rights Reserved.


#include "Player/Components/ConditionComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DemoNetDriver.h"
#include "Game/BaseGameMode.h"

DEFINE_LOG_CATEGORY_STATIC(LogConditionComponent, All, All);

UConditionComponent::UConditionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	
	Health = InitHealth;
	Armor = InitArmor;
}

void UConditionComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<APlayerCharacter>(GetOwner());
	check(Owner);

	if(UKismetSystemLibrary::IsServer(this))
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UConditionComponent::OnPlayerTakeAnyDamage);
	}
}

float UConditionComponent::GetHealth() const
{
	return Health;
}

float UConditionComponent::GetArmor() const
{
	return Armor;
}

bool UConditionComponent::IsDead() const
{
	return bIsDead;
}

bool UConditionComponent::AddHealth(float Count)
{
	if(Health == MaxHealth) return false;
	Health += Count;
	if(Health > MaxHealth)
	{
		Health = MaxHealth;
	}
	return true;
}

bool UConditionComponent::AddArmor(float Count)
{
	if(Armor == MaxArmor) return false;
	Armor += Count;
	if(Armor > MaxArmor)
	{
		Armor = MaxArmor;
	}
	return true;
}

void UConditionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UConditionComponent, Health,	 COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UConditionComponent, Armor,		 COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UConditionComponent, MaxHealth,  COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UConditionComponent, MaxArmor,	 COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UConditionComponent, InitHealth, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UConditionComponent, InitArmor,	 COND_OwnerOnly);
	DOREPLIFETIME(UConditionComponent, bIsDead);
}

void UConditionComponent::NotifyDamage_Implementation(float Damage, FVector DamageVector)
{
	OnClientDamageReceived.Broadcast(Damage, DamageVector);
}

void UConditionComponent::ApplyDamage(float Damage)
{
	const float ReceivedDamage = Damage * (1 - Armor / 200);
	Health -= ReceivedDamage;
	Armor = FMath::Max(0.f, Armor - ReceivedDamage * 2);
	UE_LOG(LogConditionComponent, Display, TEXT("Health: %f Armor: %f"), Health, Armor);
}

bool UConditionComponent::CanBeDamaged(UObject* Instigator) const
{
	if(bIsDead) return false;
	return true;
}

void UConditionComponent::OnPlayerTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	if(CanBeDamaged(InstigatedBy))
	{
		ApplyDamage(Damage);
		UE_LOG(LogConditionComponent, Display, TEXT("Apply Damage Health %f"), Health);

		NotifyDamage(Damage, FVector::ZeroVector);
		
		if(Health <= 0.f && !bIsDead)
		{
			bIsDead = true;
			OnCharDead.Broadcast();
			
			if(auto GameMode = Cast<ABaseGameMode>(UGameplayStatics::GetGameMode(this)))
			{
				GameMode->UpdateDeathPoints(Cast<APlayerController>(Owner->GetController()), Cast<APlayerController>(InstigatedBy));
				GameMode->CharacterDead(Owner, Cast<APlayerController>(InstigatedBy), DamageCauser);
			}
		}
	}
}
