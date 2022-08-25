// Fill out your copyright notice in the Description page of Project Settings.

#include "PickUp.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/SphereComponent.h"

// Sets default values
APickUp::APickUp()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(RootComponent);
	OverlapSphere->SetSphereRadius(150.f);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	PickupMesh->SetupAttachment(OverlapSphere);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void APickUp::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &APickUp::OnSphereOverlap);
	}
}

void APickUp::Destroyed()
{
	Super::Destroyed();

	if (PickupSound) {
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
	}
}

// Called every frame
void APickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APickUp::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}