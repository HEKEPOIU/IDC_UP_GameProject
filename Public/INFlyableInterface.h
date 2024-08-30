// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "INFlyableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UINFlyableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class IDC_UP_GAMEPROJECT_API IINFlyableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BeAiming(APawn* InstigatorPawn);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UnAiming(APawn* InstigatorPawn);

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void HitReact(APawn* InstigatorPawn);
};
