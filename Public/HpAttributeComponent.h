// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HpAttributeComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChange,float,NewHealth, float,MaxHealth);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IDC_UP_GAMEPROJECT_API UHpAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHpAttributeComponent();
	
	UFUNCTION(BlueprintCallable, Category="Health")
	bool ApplyHealthChange(float Delta);

	UPROPERTY(BlueprintAssignable)
	FOnHealthChange OnHealthChange;

protected:
	UPROPERTY(EditDefaultsOnly,Category="HP")
	float MaxHealth = 10;

	UPROPERTY(BlueprintReadOnly)
	float CurrentHealth;

	virtual void BeginPlay() override;
};
