// Fill out your copyright notice in the Description page of Project Settings.

#include "BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/Weapon/Weapon.h"
#include "Components/CapsuleComponent.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "BlasterAnimInstance.h"
#include "Blaster/Blaster.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/GameMode/BlasterGameMode.h"
#include "TimerManager.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Blaster/BlasterComponents/BuffComponent.h"

// Sets default values
ABlasterCharacter::ABlasterCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Component that takes care of putting the camera in the correct position
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	//Component that represtnsa the camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	//Character movement
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;

	//Combat
	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	//Buff
	Buff = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	Buff->SetIsReplicated(true);

	//Collisions
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 850.f);

	//Client/Server conection frequency
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);
	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));
}

// Called every frame
void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RotateInPlace(DeltaTime);
	HideCameraIfCharacterClose();
	PollInit();
}

///////////////////////////////////////////Death related///////////////////////////////////////////////////////////////

void ABlasterCharacter::Destroyed()
{
	Super::Destroyed();

	//if (ElimBotComponent)
	//{
	//	ElimBotComponent->DestroyComponent();
	//}
	//If the match still continues and a weapon is equipped, the weapon must be destroyed in order to prevent having 2 weapons later in that match
	ABlasterGameMode* BlasterGameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	bool bMatchNotInProgress = BlasterGameMode && BlasterGameMode->GetMatchState() != MatchState::InProgress;
	if (Combat && Combat->EquippedWeapon && bMatchNotInProgress)
	{
		Combat->EquippedWeapon->Destroy();
	}
}

void ABlasterCharacter::Elim()
{
	if (Combat && Combat->EquippedWeapon) {
		Combat->EquippedWeapon->Dropped();
	}
	MulticastElim();
	GetWorldTimerManager().SetTimer(ElimTimer, this, &ABlasterCharacter::ElimTimerFinished, ElimDelay);
}

void ABlasterCharacter::ElimTimerFinished()
{
	ABlasterGameMode* gmd = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
	if (gmd) {
		gmd->RequestRespawn(this, Controller);
	}
}

void ABlasterCharacter::MulticastElim_Implementation()
{
	if (BlasterPlayerController) {
		BlasterPlayerController->SetHUDWeaponAmmo(0);
	}
	bElim = true;
	PlayElimMontage();

	if (DissolveMaterialInstance) {
		DynamicDissolveMaterialInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstance);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Disolve"), 0.55f);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Glow"), 200.f);
	}
	StartDissolve();

	//Disable the movement component and the collision component to let the dissolve anim play
	GetCharacterMovement()->DisableMovement(); //Prevent move
	GetCharacterMovement()->StopMovementImmediately(); //Prevent rotate

	bDisableGameplay = true;
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

///////////////////////////////////////////////////Init////////////////////////////////////

// Called when the game starts or when spawned
void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateHUDHealth();
	if (HasAuthority()) {
		OnTakeAnyDamage.AddDynamic(this, &ABlasterCharacter::ReceiveDamage);
	}
}

// Called to bind functionality to input
void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Callbacks are asigned to each action
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABlasterCharacter::Jump);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ABlasterCharacter::EquipButtonPress);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ABlasterCharacter::CrouchButtonPress);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ABlasterCharacter::ReloadButtonPress);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABlasterCharacter::AimButtonPress);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABlasterCharacter::AimButtonReleased);
	//Callbacks for actions
	PlayerInputComponent->BindAxis("MoveForward", this, &ABlasterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABlasterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ABlasterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ABlasterCharacter::LookUp);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABlasterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ABlasterCharacter::FireButtonReleased);

	int intVar = 5;
	float floatVar = 3.7f;
	FString fstringVar = "Llamado desde inicio";
	UE_LOG(LogTemp, Warning, TEXT("Text, %d %f %s"), intVar, floatVar, *fstringVar);
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterCharacter, CurrentHealth);
	DOREPLIFETIME(ABlasterCharacter, bDisableGameplay);
	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat) {
		Combat->Character = this;
	}

	if (Buff) {
		Buff->Character = this;
	}
}

void ABlasterCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;
}

void ABlasterCharacter::PollInit()
{
	if (BlasterPlayerState == nullptr) {
		BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
		if (BlasterPlayerState) {
			BlasterPlayerState->AddToScore(0);
			BlasterPlayerState->AddToDefeats(0);
		}
	}
}

////////////////////////////////////////////Input/////////////////////////////////////////////

void ABlasterCharacter::EquipButtonPress()
{
	if (bDisableGameplay)return;

	if (HasAuthority())
		Combat->EquipWeapon(OverlappingWeapon);
	else
		ServerEquipButtonPress();
}

void ABlasterCharacter::CrouchButtonPress()
{
	if (bDisableGameplay)return;

	if (bIsCrouched)
		UnCrouch();
	else
		Crouch();
}
void ABlasterCharacter::ReloadButtonPress()
{
	if (bDisableGameplay)return;

	int intVar = 5;
	float floatVar = 3.7f;
	FString fstringVar = "Llamado al ser pulsado el boton de recargar";
	UE_LOG(LogTemp, Warning, TEXT("Text, %d %f %s"), intVar, floatVar, *fstringVar);
	if (Combat) {
		Combat->Reload();
	}
}

void ABlasterCharacter::AimButtonPress()
{
	if (bDisableGameplay)return;

	if (Combat) {
		Combat->SetAiming(true);
	}
}

void ABlasterCharacter::AimButtonReleased()
{
	if (bDisableGameplay)return;

	if (Combat) {
		Combat->SetAiming(false);
	}
}

void ABlasterCharacter::FireButtonPressed()
{
	if (bDisableGameplay)return;

	if (Combat) {
		Combat->FireButtonPressed(true);
	}
}

void ABlasterCharacter::FireButtonReleased()
{
	if (bDisableGameplay)return;

	if (Combat) {
		Combat->FireButtonPressed(false);
	}
}

void ABlasterCharacter::ServerEquipButtonPress_Implementation()
{
	if (Combat) {
		Combat->EquipWeapon(OverlappingWeapon);
	}
}

////////////////////////////////////////////////////Actions////////////////////////////////////////////////////

void ABlasterCharacter::Jump()
{
	if (bDisableGameplay)return;
	if (bIsCrouched) {
		UnCrouch();
	}
	else {
		Super::Jump();
	}
}

void ABlasterCharacter::MoveForward(float Value)
{
	if (bDisableGameplay)return;
	if (Controller != nullptr && Value != 0.f) {
		const FRotator  YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));

		AddMovementInput(Direction, Value);
	}
}

void ABlasterCharacter::MoveRight(float Value)
{
	if (bDisableGameplay)return;

	if (Controller != nullptr && Value != 0.f) {
		const FRotator  YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

		AddMovementInput(Direction, Value);
	}
}

void ABlasterCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ABlasterCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void ABlasterCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DType, AController* InstigatorController, AActor* DamageCauser)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);

	UpdateHUDHealth();
	PlayHitReactMontage();

	if (CurrentHealth <= 0.f) {
		ABlasterGameMode* gmd = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
		if (gmd) {
			BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
			ABlasterPlayerController* otherController = Cast<ABlasterPlayerController>(InstigatorController);
			gmd->PlayerEliminated(this, BlasterPlayerController, otherController);
		}
	}
}

void ABlasterCharacter::RotateInPlace(float DeltaTime)
{
	if (bDisableGameplay) {
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;

		return;
	}

	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else {
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f) {
			OnRep_ReplicateMovement();
		}
		CalculateAO_Pitch();
	}
}

void ABlasterCharacter::TurnInplace(float DeltaTime)
{
	if (AO_Yaw > 90.f) {
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	if (AO_Yaw < -90.f) {
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}

	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning) {
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 5.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f) {
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

///////////////////////////////////////////////Animations/////////////////////////////////////////////////////////////

void ABlasterCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* animinstance = GetMesh()->GetAnimInstance();
	if (animinstance && FireWeaponMontage) {
		animinstance->Montage_Play(FireWeaponMontage);
		FName sectionname;
		sectionname = bAiming ? FName("RifleAim") : FName("RifleHip");
		animinstance->Montage_JumpToSection(sectionname);
	}
}

void ABlasterCharacter::PlayReloadMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	int intVar = 5;
	float floatVar = 3.7f;
	FString fstringVar = "LLamado al iniciar animacion";
	UE_LOG(LogTemp, Warning, TEXT("Text, %d %f %s"), intVar, floatVar, *fstringVar);

	UAnimInstance* animinstance = GetMesh()->GetAnimInstance();
	if (animinstance && ReloadMontage) {
		animinstance->Montage_Play(ReloadMontage);
		FName sectionname;
		switch (Combat->EquippedWeapon->GetWeaponType()) {
		case EWeaponType::EWT_AssaultRifle:
			sectionname = FName("Rifle");
			break;
		case EWeaponType::EWT_RocketLauncher:
			sectionname = FName("Rifle");
			break;
		case EWeaponType::EWT_Pistol:
			sectionname = FName("Rifle");
			break;
		}
		animinstance->Montage_JumpToSection(sectionname);
	}
}

void ABlasterCharacter::PlayElimMontage()
{
	UAnimInstance* animinstance = GetMesh()->GetAnimInstance();
	if (animinstance && ElimMontage) {
		animinstance->Montage_Play(ElimMontage);
	}
}

void ABlasterCharacter::PlayHitReactMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* animinstance = GetMesh()->GetAnimInstance();
	if (animinstance && HitReactMontage) {
		animinstance->Montage_Play(HitReactMontage);
		FName sectionname = FName("FromFront");
		animinstance->Montage_JumpToSection(sectionname);
	}
}

float ABlasterCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	float Speed = Velocity.Size();
	return Speed;
}

void ABlasterCharacter::AimOffset(float DeltaTime)
{
	if (Combat && Combat->EquippedWeapon == nullptr) return;
	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir) // standing still, not jumping
	{
		bRotateRootBone = true;
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning) {
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;

		//Take care of turning
		TurnInplace(DeltaTime);
	}
	if (Speed > 0.f || bIsInAir) // running, or jumping
	{
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	CalculateAO_Pitch();
}

void ABlasterCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		// map pitch from [270, 360) to [-90, 0)
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void ABlasterCharacter::SimProxiesTurn()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr)return;

	float Speed = CalculateSpeed();
	bRotateRootBone = false;
	if (Speed > 0.f) {
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	ProxyRotationLastFrame = ProxyRotation;
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;
	if (FMath::Abs(ProxyYaw) > TurnThreshold) {
		if ((ProxyYaw) > TurnThreshold) {
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if ((ProxyYaw) < -TurnThreshold) {
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else {
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
}

//////////////////////////////////Weapon overlapping/////////////////////////////////////
void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon) OverlappingWeapon->ShowPickupWidget(false);

	OverlappingWeapon = Weapon;
	if (IsLocallyControlled()) {
		if (OverlappingWeapon)
			OverlappingWeapon->ShowPickupWidget(true);
	}
}

void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon) {
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
		LastWeapon->ShowPickupWidget(false);
}

//////////////////////////////////////////////Health//////////////////////////////////////////

void ABlasterCharacter::UpdateHUDHealth()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController) {
		BlasterPlayerController->SetHUDHelth(CurrentHealth, MaxHealth);
	}
}

void ABlasterCharacter::OnRep_Health(float lastHealth)
{
	UpdateHUDHealth();
	if (CurrentHealth < lastHealth) {
		PlayHitReactMontage();
	}
}

void ABlasterCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled()) return;
	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshHold) {
		GetMesh()->SetVisibility(false);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh()) {
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else {
		GetMesh()->SetVisibility(true);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh()) {
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

////////////////////////////////////DIsolve material/////////////////////////////////////////////

void ABlasterCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	if (DynamicDissolveMaterialInstance) {
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}
}

void ABlasterCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &ABlasterCharacter::UpdateDissolveMaterial);
	if (DissolveCurve && DissolveTimeline) {
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
}

/////////////////////////////////////////Getters////////////////////////////////////////////

bool ABlasterCharacter::IsWeaponEquipped() {
	return Combat && Combat->EquippedWeapon;
}

bool ABlasterCharacter::IsAiming() {
	return Combat && Combat->bAiming;
}

AWeapon* ABlasterCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr)
		return nullptr;
	else return Combat->EquippedWeapon;
}

FVector ABlasterCharacter::GetHitTarget() const
{
	if (Combat == nullptr)
		return FVector();
	return Combat->HitTarget;
}

ECombatState ABlasterCharacter::GetCombatState()
{
	if (Combat == nullptr) return ECombatState::ECS_MAX;
	return Combat->CombatState;
}