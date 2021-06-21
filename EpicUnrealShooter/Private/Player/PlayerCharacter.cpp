// Copyright Saber Studio. All Rights Reserved.


#include "Player/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/ConditionComponent.h"
#include "Components/WeaponComponent.h"
#include "Engine/DemoNetDriver.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapons/FireWeapon.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlayerCharacter, All, All);

APlayerCharacter::APlayerCharacter()
{
 	PrimaryActorTick.bCanEverTick = true;
	NetUpdateFrequency = 10;
	
	GetMesh()->bOwnerNoSee = true;

	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));

	GetCapsuleComponent()->SetIsReplicated(true);
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);
	Camera->SetRelativeLocation(FVector(10.f, 0.f, 80.f));
	Camera->bUsePawnControlRotation = true;
	
	ConditionComponent = CreateDefaultSubobject<UConditionComponent>(TEXT("ConditionComponent"));

	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));
	
	auto MovementComponent = ACharacter::GetMovementComponent();
	if(MovementComponent)
	{
		MovementComponent->GetNavAgentPropertiesRef().bCanCrouch = true;
		MovementComponent->SetIsReplicated(true);
	}
	
}

UWeaponComponent* APlayerCharacter::GetWeaponComponent()
{
	return WeaponComponent;
}

const UCameraComponent* APlayerCharacter::GetCamera() const
{
	return Camera;
}

USkeletalMeshComponent* APlayerCharacter::GetVisibleMesh() const
{
	return GetMesh();
}

bool APlayerCharacter::AddHealth(float Count)
{
	return ConditionComponent->AddHealth(Count);
}

bool APlayerCharacter::AddArmor(float Count)
{
	return ConditionComponent->AddArmor(Count);
}

bool APlayerCharacter::AddAmmo()
{
	return WeaponComponent->AddAmmo();
}

void APlayerCharacter::SetFieldOfView(float Value)
{
	Camera->FieldOfView = Value;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	check(GetCharacterMovement());
	check(WeaponComponent);
	check(ConditionComponent);
	
	ConditionComponent->OnCharDead.AddDynamic(this, &APlayerCharacter::Death);

	if(HasAuthority())
	{
		for(const auto e : WeaponComponent->WeaponClasses)
		{
			WeaponComponent->AddWeapon(WeaponComponent->CreateWeapon(e));
		}
	}
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayerCharacter, bDead);
	DOREPLIFETIME(APlayerCharacter, BodyMaterial);
	DOREPLIFETIME(APlayerCharacter, HeadMaterial);
	DOREPLIFETIME(APlayerCharacter, EyesMaterial);
}

void APlayerCharacter::Reload()
{
	if(WeaponComponent->CurrentWeapon)
	{
		if(HasAuthority())
		{
			auto LFireWeapon = Cast<AFireWeapon>(WeaponComponent->CurrentWeapon);
			if(LFireWeapon && LFireWeapon->CanReloadWeapon())
			{
				LFireWeapon->ReloadWeapon();
			}
		}
		else
		{
			ServerReload();
		}
	}
}

void APlayerCharacter::ServerReload_Implementation()
{
	Reload();
}

void APlayerCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void APlayerCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

void APlayerCharacter::StartSecondAction()
{
	if(WeaponComponent->CurrentWeapon)
		WeaponComponent->CurrentWeapon->SecondActionStart();
}

void APlayerCharacter::StopSecondAction()
{
	if(WeaponComponent->CurrentWeapon)
		WeaponComponent->CurrentWeapon->SecondActionStop();
}

void APlayerCharacter::StartFire()
{
	if(WeaponComponent->CurrentWeapon && WeaponComponent->CanFire())
	{
		if(HasAuthority())
		{
			WeaponComponent->CurrentWeapon->StartFire();
		}
		else
		{
			if(WeaponComponent->CurrentWeapon->CanUseWeapon())
			{
				ServerStartFire();
			}
		}
	}
}

void APlayerCharacter::StopFire()
{
	if(WeaponComponent->CurrentWeapon)
	{
		if(HasAuthority())
		{
			WeaponComponent->CurrentWeapon->StopFire();
		}
		else
		{
			ServerStopFire();
		}
	}
}

void APlayerCharacter::ServerStartFire_Implementation()
{
	StartFire();
}

void APlayerCharacter::ServerStopFire_Implementation()
{
	StopFire();
}

void APlayerCharacter::Jump()
{
	Super::Jump();
}

void APlayerCharacter::StopJumping()
{
	Super::StopJumping();
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",   this, &APlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", 	  this, &APlayerCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookRight",   this, &APlayerCharacter::AddControllerYawInput);
 
	PlayerInputComponent->BindAction("Fire",   IE_Pressed,  this, &APlayerCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire",   IE_Released, this, &APlayerCharacter::StopFire);

	PlayerInputComponent->BindAction("SecondAction",   IE_Pressed,  this, &APlayerCharacter::StartSecondAction);
	PlayerInputComponent->BindAction("SecondAction",   IE_Released, this, &APlayerCharacter::StopSecondAction);
	
	PlayerInputComponent->BindAction("Jump",   IE_Pressed,  this, &APlayerCharacter::Jump);
	PlayerInputComponent->BindAction("Jump",   IE_Released, this, &APlayerCharacter::StopJumping);

	PlayerInputComponent->BindAction("Reload",   IE_Pressed,  this, &APlayerCharacter::Reload);

	PlayerInputComponent->BindAction("Pistol",			IE_Pressed,  this, &APlayerCharacter::SelectPistol);
	PlayerInputComponent->BindAction("Rifle",			IE_Pressed,  this, &APlayerCharacter::SelectRifle);
	PlayerInputComponent->BindAction("Shotgun",			IE_Pressed,  this, &APlayerCharacter::SelectShotgun);
	PlayerInputComponent->BindAction("Launcher",		IE_Pressed,  this, &APlayerCharacter::SelectLauncher);
	PlayerInputComponent->BindAction("SniperRifle",		IE_Pressed,  this, &APlayerCharacter::SelectSniperRifle);
	PlayerInputComponent->BindAction("RocketLauncher",  IE_Pressed,  this, &APlayerCharacter::SelectRocketLauncher);
	PlayerInputComponent->BindAction("Grenade",		    IE_Pressed,  this, &APlayerCharacter::SelectGrenade);
	PlayerInputComponent->BindAction("Knife",	     	IE_Pressed,  this, &APlayerCharacter::SelectKnife);
}

void APlayerCharacter::OnRep_BodyMaterial()
{
	GetMesh()->SetMaterial(0, BodyMaterial);
}

void APlayerCharacter::OnRep_HeadMaterial()
{
	GetMesh()->SetMaterial(2, HeadMaterial);
}

void APlayerCharacter::OnRep_EyesMaterial()
{
	GetMesh()->SetMaterial(3, EyesMaterial);
}

void APlayerCharacter::OnRep_bDead()
{
	auto LMesh = GetMesh();
	if(LMesh)
	{
		LMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		LMesh->SetSimulatePhysics(true);
	}
}

void APlayerCharacter::Death()
{
	OnCharacterDead.Broadcast();
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->DestroyComponent();
	if(HasAuthority())
	{
		SetLifeSpan(DestroyTimeSeconds);
	}
	bDead = true;
	DestroyWeapons();
}

void APlayerCharacter::DestroyWeapons_Implementation()
{
	if(WeaponComponent->Pistol) WeaponComponent->Pistol->Destroy();
	if(WeaponComponent->Rifle) WeaponComponent->Rifle->Destroy();
	if(WeaponComponent->Shotgun) WeaponComponent->Shotgun->Destroy();
	if(WeaponComponent->GrenadeLauncher) WeaponComponent->GrenadeLauncher->Destroy();
	if(WeaponComponent->SniperRifle) WeaponComponent->SniperRifle->Destroy();
	if(WeaponComponent->RocketLauncher) WeaponComponent->RocketLauncher->Destroy();
	if(WeaponComponent->Grenade) WeaponComponent->Grenade->Destroy();
	if(WeaponComponent->Knife) WeaponComponent->Knife->Destroy();
}

void APlayerCharacter::SelectWeapon(EWeaponType Type)
{
	if(Type == WeaponComponent->CurrentWeapon->GetWeaponType()) return;
	StopFire();
	if(HasAuthority())
	{
		WeaponComponent->SelectWeapon(nullptr, Type);
	}
	else
	{
		if(WeaponComponent->CanEquip())
		{
			ServerSelectWeapon(Type);
		}
	}
}

void APlayerCharacter::ServerSelectWeapon_Implementation(EWeaponType Type)
{
	SelectWeapon(Type);
}

void APlayerCharacter::SelectPistol()
{
	SelectWeapon(EWeaponType::Pistol);
}

void APlayerCharacter::SelectRifle()
{
	SelectWeapon(EWeaponType::Rifle);
}

void APlayerCharacter::SelectShotgun()
{
	SelectWeapon(EWeaponType::Shotgun);
}

void APlayerCharacter::SelectLauncher()
{
	SelectWeapon(EWeaponType::GrenadeLauncher);
}
void APlayerCharacter::SelectSniperRifle()
{
	SelectWeapon(EWeaponType::SniperRifle);
}

void APlayerCharacter::SelectRocketLauncher()
{
	SelectWeapon(EWeaponType::RocketLauncher);
}

void APlayerCharacter::SelectGrenade()
{
	SelectWeapon(EWeaponType::Grenade);
}

void APlayerCharacter::SelectKnife()
{
	SelectWeapon(EWeaponType::Knife);
}
