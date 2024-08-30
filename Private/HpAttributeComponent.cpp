// Fill out your copyright notice in the Description page of Project Settings.


#include "HpAttributeComponent.h"

// Sets default values for this component's properties
UHpAttributeComponent::UHpAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
}

bool UHpAttributeComponent::ApplyHealthChange(float Delta)
{
	CurrentHealth += Delta;
	CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);
	OnHealthChange.Broadcast(CurrentHealth, MaxHealth);
	return CurrentHealth > 0;
}

void UHpAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
}




