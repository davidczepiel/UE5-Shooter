// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LagCompensationComponent.generated.h"

USTRUCT(BlueprintType)
struct FBoxInformation
{
	GENERATED_BODY()
		UPROPERTY()		FVector Location;
	UPROPERTY()			FRotator Rotation;
	UPROPERTY()			FVector BoxExtent;
};

USTRUCT(BlueprintType)
struct FFramePackage
{
	GENERATED_BODY()
		UPROPERTY()		float Time;
	UPROPERTY()			TMap<FName, FBoxInformation> HitBoxInfo;
	UPROPERTY()			ABlasterCharacter* Character;
};

USTRUCT(BlueprintType)
struct FServerSideRewindResult
{
	GENERATED_BODY()
		UPROPERTY()		bool bHitConfirmed;
	UPROPERTY()			bool bHeadShot;
};

USTRUCT(BlueprintType)
struct FShotgunServerSideRewindResult
{
	GENERATED_BODY()
		UPROPERTY()		TMap<ABlasterCharacter*, uint32> HeadShots;
	UPROPERTY()			TMap<ABlasterCharacter*, uint32> BodyShots;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BLASTER_API ULagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULagCompensationComponent();
	friend class ABlasterCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// <summary>
	/// Funciton that allows to rende the hitboxes of a character at a given time with a specified color
	/// </summary>
	/// <param name="Package"> Information of the player hitboxes that is going to be rendered </param>
	/// <param name="Color"> Color to use when rendering the hitboxes </param>
	void ShowFramePackage(const FFramePackage& Package, const FColor& Color);

	/// <summary>
	/// Function that  confirms a hitscan hit that they did on their machine on a specific timestamp to a given character
	/// </summary>
	/// <param name="HitCharacter"></param>
	/// <param name="TraceStart"></param>
	/// <param name="HitLocation"></param>
	/// <param name="HitTime"></param>
	/// <returns></returns>
	FServerSideRewindResult ServerSideRewind(class ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime);

	/// <summary>
	/// Function that  confirms a projectile hit that they did on their machine on a specific timestamp to a given character
	/// </summary>
	/// <param name="HitCharacter"></param>
	/// <param name="TraceStart"></param>
	/// <param name="HitLocation"></param>
	/// <param name="HitTime"></param>
	/// <returns></returns>
	FServerSideRewindResult ProjectileServerSideRewind(ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime);

	/**
	* Shotgun
	*/
	FShotgunServerSideRewindResult ShotgunServerSideRewind(const TArray<ABlasterCharacter*>& HitCharacters, const FVector_NetQuantize& TraceStart, const TArray<FVector_NetQuantize>& HitLocations, float HitTime);

	/// <summary>
	/// RPC that the clients can call to ask the server to confirm a hitscan hit that they did on their machine on a specific timestamp to a given character
	/// </summary>
	/// <param name="HitCharacter"> Character that the client hit </param>
	/// <param name="TraceStart"> Client shooting start </param>
	/// <param name="HitLocation"> World point where the hit happened </param>
	/// <param name="HitTime"> Time stamp when the hit happened </param>
	/// <returns></returns>
	UFUNCTION(Server, Reliable)		void ServerScoreRequest(ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime);

	/// <summary>
	/// RPC that the clients can call to ask the server to confirm a projectile hit that they did on their machine on a specific timestamp to a given character
	/// </summary>
	/// <param name="HitCharacter"> Character that the client hit </param>
	/// <param name="TraceStart"> Client shooting start </param>
	/// <param name="HitLocation"> World point where the hit happened </param>
	/// <param name="HitTime"> Time stamp when the hit happened </param>
	/// <returns></returns>
	UFUNCTION(Server, Reliable)		void ProjectileServerScoreRequest(ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime);

	UFUNCTION(Server, Reliable)		void ShotgunServerScoreRequest(const TArray<ABlasterCharacter*>& HitCharacters, const FVector_NetQuantize& TraceStart, const TArray<FVector_NetQuantize>& HitLocations, float HitTime);

protected:
	virtual void BeginPlay() override;
	/// <summary>
	/// Function that takes care of the history of hitbox information stored for the character, adds new packages and removes the ones too old for future hit confirmations
	/// </summary>
	void SaveFramePackage();
	/// <summary>
	/// Function that is going to save a characters hitboxes information for further hit confirmation
	/// </summary>
	void SaveFramePackage(FFramePackage& Package);

	/// <summary>
	/// Function that takes the two frames betwean a timestamp asked ant interps the hitbox information betwean both frames to get the exact location of every hitbox to be able to confirm a hit
	/// </summary>
	/// <param name="OlderFrame"></param>
	/// <param name="YoungerFrame"></param>
	/// <param name="HitTime"></param>
	/// <returns></returns>
	FFramePackage InterpBetweenFrames(const FFramePackage& OlderFrame, const FFramePackage& YoungerFrame, float HitTime);
	/// <summary>
	/// Function that has an out parameter where the characters hitbox information will be stored
	/// </summary>
	/// <param name="HitCharacter"> Character to get the info from </param>
	/// <param name="OutFramePackage"> Out parameter that will get the hitboxes info </param>
	void CacheBoxPositions(ABlasterCharacter* HitCharacter, FFramePackage& OutFramePackage);
	/// <summary>
	/// Function that moves a specified characters hitboxes to the position and rotation given by another parameter
	/// </summary>
	/// <param name="HitCharacter"> Character that is going to be modified </param>
	/// <param name="Package"> Parameter with the info to move the character </param>
	void MoveBoxes(ABlasterCharacter* HitCharacter, const FFramePackage& Package);
	/// <summary>
	/// Function that takes a character and moves its hitboxes to match the info given in the package variable
	/// </summary>
	/// <param name="HitCharacter"> Character that is going to be modified </param>
	/// <param name="Package"> package where the info to move the character is going to be taken from </param>
	void ResetHitBoxes(ABlasterCharacter* HitCharacter, const FFramePackage& Package);
	/// <summary>
	/// Function that sets all the hitboxes of a given character to react to a specific given vollision type
	/// </summary>
	/// <param name="HitCharacter"> Character whose hitboxes are going to be modified </param>
	/// <param name="CollisionEnabled"> Type of collision that the hitboxes are going to respond to </param>
	void EnableCharacterMeshCollision(ABlasterCharacter* HitCharacter, ECollisionEnabled::Type CollisionEnabled);

	/// <summary>
	/// Function that takes a history of frame packages saved and returns the framepackage that corresponds to the given timestamp when the hit happened
	/// </summary>
	/// <param name="HitCharacter"> Character that got hit </param>
	/// <param name="HitTime"> Timestamp when the hit happened </param>
	/// <returns></returns>
	FFramePackage GetFrameToCheck(ABlasterCharacter* HitCharacter, float HitTime);

	/// <summary>
	/// Function that confirms if a hitscan trace landed on a clients machine
	/// </summary>
	/// <param name="Package"> Out frame packege that will contain the characters boxes moved to a specific position</param>
	/// <param name="HitCharacter"> Character that maybe got hit </param>
	/// <param name="TraceStart"> Start of the trace </param>
	/// <param name="HitLocation"> Hit that client did </param>
	/// <returns></returns>
	FServerSideRewindResult ConfirmHit(const FFramePackage& Package, ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation);

	/**
	* Projectile
	*/
	FServerSideRewindResult ProjectileConfirmHit(const FFramePackage& Package, ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime);

	/**
	* Shotgun
	*/
	FShotgunServerSideRewindResult ShotgunConfirmHit(const TArray<FFramePackage>& FramePackages, const FVector_NetQuantize& TraceStart, const TArray<FVector_NetQuantize>& HitLocations);

private:
	UPROPERTY()					ABlasterCharacter* Character;
	UPROPERTY()					class ABlasterPlayerController* Controller;
	UPROPERTY(EditAnywhere)		float MaxRecordTime = 4.f;
	TDoubleLinkedList<FFramePackage> FrameHistory;
};