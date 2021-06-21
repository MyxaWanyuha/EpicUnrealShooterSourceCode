// Copyright Saber Studio. All Rights Reserved.


#include "Weapons/BaseProjectile.h"



#include "DrawDebugHelpers.h"
#include "PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DemoNetDriver.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"

ABaseProjectile::ABaseProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	NetUpdateFrequency = 10;
	AActor::SetReplicateMovement(true);
	SetReplicates(true);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	CollisionBox->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
	CollisionBox->BodyInstance.bNotifyRigidBodyCollision = true;
	SetRootComponent(CollisionBox);
	
	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->InitialSpeed = 2000.0f;
	MovementComponent->ProjectileGravityScale = GravityScale;
}

void ABaseProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseProjectile, Weapon);
	DOREPLIFETIME(ABaseProjectile, bPlaySound);
}

void ABaseProjectile::OnRep_PlaySound()
{
	UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation(), GetActorRotation());
}

void ABaseProjectile::PostDestroy()
{
	Destroy();
}

void ABaseProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	const auto World = GetWorld();
	if (!World) return;
	
	if(HasAuthority())
	{
		bPlaySound = !bPlaySound;
		MovementComponent->StopMovementImmediately();

		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		ActorSpawnParams.Owner = GetOwner();
		auto FireHole = World->SpawnActor<AFireHole>(FireHoleClass, GetActorLocation(), GetActorRotation(), ActorSpawnParams);
		FireHole->SetLifeSpan(30.f);
		FireHole->SetActorRotation(UKismetMathLibrary::MakeRotFromX(Hit.Normal));
		
		auto b = UGameplayStatics::ApplyRadialDamage(World,
            Damage,
            GetActorLocation(),
            DamageRadius,
            UDamageType::StaticClass(),
            {},
            this,
            GetController(),
            DoFullDamage);

		GetWorldTimerManager().SetTimer(DestroyTimer, this, &ABaseProjectile::PostDestroy, 0.1f);
	}
}

void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();
	check(CollisionBox);

	if(Weapon)
	{
		Weapon->GetWeaponOwner()->GetCapsuleComponent()->IgnoreActorWhenMoving(this, true);
		CollisionBox->IgnoreActorWhenMoving(Weapon->GetWeaponOwner(), true);
	}
	if(HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &ABaseProjectile::OnProjectileHit);
	}
	SetLifeSpan(LifeSeconds);
}

AController* ABaseProjectile::GetController() const
{
	const auto Pawn = Cast<APawn>(Weapon->GetWeaponOwner());
	return Pawn ? Pawn->GetController() : nullptr;
}