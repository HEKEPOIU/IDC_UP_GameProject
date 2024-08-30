// Fill out your copyright notice in the Description page of Project Settings.


#include "INProjectileParent.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"


// Sets default values
AINProjectileParent::AINProjectileParent()
{
	SphereMeshComponent= CreateDefaultSubobject<UStaticMeshComponent>("SphereMesh");
	RootComponent = SphereMeshComponent;

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComp");
	MovementComp->InitialSpeed = 3000;
	MovementComp->bInitialVelocityInLocalSpace = true;
	MovementComp->MaxSpeed = 3500;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->ProjectileGravityScale = 0.0f;

	SphereCollision = CreateDefaultSubobject<USphereComponent>("SphereCollision");
	SphereCollision->SetupAttachment(RootComponent);
	SphereCollision->SetCollisionProfileName("Aimable");
	
	
	
}




