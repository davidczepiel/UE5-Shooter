// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"

//Physics layers for specific purposes
#define ECC_SkeletalMesh ECollisionChannel::ECC_GameTraceChannel1		//
#define ECC_HitBox ECollisionChannel::ECC_GameTraceChannel2				//Hitboxes for Server Side Rewind