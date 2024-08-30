// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "INFlyableInterface.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "INProjectileParent.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS(Abstract)
class IDC_UP_GAMEPROJECT_API AINProjectileParent : public AActor ,public IINFlyableInterface
{
	GENERATED_BODY()

public:

	AINProjectileParent();

protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UStaticMeshComponent* SphereMeshComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UProjectileMovementComponent* MovementComp;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	USphereComponent* SphereCollision;
	
};
