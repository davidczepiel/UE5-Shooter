// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Animation/AnimationAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "Casing.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"

// Sets default values
AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	//Weapon mesh adjustments
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(WeaponMesh);

	//Trigger to be picked up
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//Pickup widget for when the weapon is in the floor and can be picked up by a player
	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	//If a pick up widget is avaliable it is inmediatelly turned on
	if (PickupWidget)		PickupWidget->SetVisibility(false);

	//The weapon has collision properties only on the server
	if (HasAuthority()) {
		//Trigger adjustments
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

		//Callbacks for on trigger enter/exit
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnSphereEndOverlap);
	}
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::ShowPickupWidget(bool bShowWidget)
{
	if (PickupWidget) 	PickupWidget->SetVisibility(bShowWidget);
}

void AWeapon::Dropped()
{
	//State is changed
	SetWeaponState(EWeaponState::EWS_Dropped);
	//The weapon is detached from the player and throwed to the ground
	FDetachmentTransformRules Detachrules(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(Detachrules);
	//Variables are cleaned to prevent unwanted behaviour
	SetOwner(nullptr);
	OwnerCharacter = nullptr;
	OwnerController = nullptr;
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//It is important to replicate state and ammo for all of the clients when a change is made at the server
	DOREPLIFETIME(AWeapon, WeaponState);
	DOREPLIFETIME(AWeapon, CurrentAmmo);
}

void AWeapon::OnRep_WeaponState()
{
	switch (WeaponState) {
	case EWeaponState::EWS_Equiped:
		ShowPickupWidget(false);
		//NO collision when equiped
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::EWS_Dropped:
		//Collisions are activated
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}
}

void AWeapon::OnRep_Ammo()
{
	OwnerCharacter = OwnerCharacter == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : OwnerCharacter;
	SetHUDWeaponAmmo();
}

void AWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();

	if (Owner == nullptr) {
		OwnerCharacter = nullptr;
		OwnerController = nullptr;
	}
	//Aditional information is set to the players HUD
	else {
		SetHUDWeaponAmmo();
	}
}

/////////////////////////////////////////////AMO RELATED///////////////////////////////////////////////////

void AWeapon::Fire(const FVector& HitTarget)
{
	//Weapon animation
	if (FireAnimation) 	WeaponMesh->PlayAnimation(FireAnimation, false);
	//If a casing was provided a new casing is spawned from the weapon
	if (CasingClass) {
		const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName(FName(TEXT("AmmoEject")));
		if (AmmoEjectSocket) {
			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMesh);

			UWorld* world = GetWorld();
			if (world) 			world->SpawnActor<ACasing>(CasingClass, SocketTransform.GetLocation(), SocketTransform.GetRotation().Rotator());
		}
	}

	SpendRound();
}

void AWeapon::SpendRound()
{
	CurrentAmmo = FMath::Clamp(CurrentAmmo - 1, 0, MaxAmmo);
	SetHUDWeaponAmmo();
}

void AWeapon::AddAmmo(int32 Amount)
{
	CurrentAmmo = FMath::Clamp(CurrentAmmo - Amount, 0, MaxAmmo);
	SetHUDWeaponAmmo();
}

void AWeapon::SetHUDWeaponAmmo()
{
	OwnerCharacter = OwnerCharacter == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : OwnerCharacter;
	//If the weapon is currently being used by a player, its ammo is displayed on the players HUD
	if (OwnerCharacter) {
		OwnerController = OwnerController == nullptr ? Cast<ABlasterPlayerController>(OwnerCharacter->Controller) : OwnerController;
		if (OwnerController) OwnerController->SetHUDWeaponAmmo(CurrentAmmo);
	}
}

void AWeapon::SetWeaponState(EWeaponState NewState)
{
	WeaponState = NewState;
	switch (WeaponState) {
	case EWeaponState::EWS_Equiped:
		ShowPickupWidget(false);
		//NO collision when equiped
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::EWS_Dropped:
		//Collisions are activated
		if (HasAuthority()) AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}
}

USphereComponent* AWeapon::GetAreaShpere() {
	return AreaSphere;
}

void AWeapon::OnSphereOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	//Actor that has overlapped with weapon is casted to see if it was actually a player, if so, the pick up widget is displayed
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter) 	BlasterCharacter->SetOverlappingWeapon(this);
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//Actor that has ended overlapping with weapon is casted to see if it was actually a player, if so, the pick up widget is hidden
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter) 	BlasterCharacter->SetOverlappingWeapon(nullptr);
}