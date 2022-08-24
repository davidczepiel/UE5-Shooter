// Fill out your copyright notice in the Description page of Project Settings.

#include "Casing.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
ACasing::ACasing()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(CasingMesh);
	CasingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	CasingMesh->SetNotifyRigidBodyCollision(true);
	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetEnableGravity(true);
	ShellEjectionImpulse = 10.f;
}

// Called every frame
void ACasing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called when the game starts or when spawned
void ACasing::BeginPlay()
{
	Super::BeginPlay();

	//Sets the callback for when the casing hits something to destroy itself
	CasingMesh->OnComponentHit.AddDynamic(this, &ACasing::OnHit);
	//Casing is ejected from the weapon
	CasingMesh->AddImpulse(GetActorForwardVector() * ShellEjectionImpulse);
}

void ACasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ShellSound) UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
	Destroy();
}