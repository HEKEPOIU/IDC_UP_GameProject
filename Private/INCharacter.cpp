// Fill out your copyright notice in the Description page of Project Settings.


#include "INCharacter.h"
#include "INFlyAbilityComponent.h"
#include "INFlyableInterface.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AINCharacter::AINCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp ->SetupAttachment(RootComponent);
	SpringArmComp ->bUsePawnControlRotation = true;
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp ->SetupAttachment(SpringArmComp);

	FlyAbilityComp = CreateDefaultSubobject<UINFlyAbilityComponent>("FlyAbilityComp");

	//設定預設值
	bAimingState = false;
	MaxChargeTime = 1;

	SprintAdditionalSpeed = 100.0f;
}


void AINCharacter::Moveforward(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Roll	= 0.0f;
	ControlRot.Pitch = 0.0f;
	AddMovementInput(ControlRot.Vector(),Value);
	
}

void AINCharacter::MoveRight(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Roll	= 0.0f;
	ControlRot.Pitch = 0.0f;

	FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);
	AddMovementInput(RightVector,Value);
}

void AINCharacter::Jump()
{
	if (FlyAbilityComp->CurrentFlyingState == FlyingState::Ve_OnFalling
			&& FlyAbilityComp->SlowDownTime>0.0f
			&& !bAimingState)
	{
		//開啟計時器
		FlyAbilityComp->SlowDownCountDown();
		//TODO:顯示降技能與否的UI
		UE_LOG(LogTemp, Log, TEXT("緩降發動"));
	}
	else if(FlyAbilityComp->SlowDownTime<0.0f)
	{
		UE_LOG(LogTemp, Log, TEXT("剩餘時間不足"));
	}
	else
	{
		Super::Jump();
	}
}

void AINCharacter::SlowDownRelease()
{
	//清除計時器
	GetOwner()->GetWorld()->GetTimerManager().ClearTimer(FlyAbilityComp->TimerHandle_SlowDown);
	
	if(FlyAbilityComp->CurrentFlyingState != FlyingState::Ve_OnDropSkill
			&&FlyAbilityComp->CurrentFlyingState != FlyingState::Ve_Normal)
	{
		FlyAbilityComp->Falling();
	}
}


void AINCharacter::DropSkill()
{
	if(FlyAbilityComp->CurrentFlyingState == FlyingState::Ve_OnSlowDown
		|| FlyAbilityComp->CurrentFlyingState == FlyingState::Ve_OnFalling)
	{
		FlyAbilityComp->Drop();
		UE_LOG(LogTemp, Log, TEXT("墜落技能發動"));
	}
}


void AINCharacter::StartSprint()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		GetCharacterMovement()->MaxWalkSpeed += SprintAdditionalSpeed;
		bIsRunning = true;
	}
}

void AINCharacter::EndSprint()
{
	if (bIsRunning)
	{
		GetCharacterMovement()->MaxWalkSpeed -= SprintAdditionalSpeed;
		bIsRunning = false;
	}
}


void AINCharacter::AimingStart()
{
	if(FlyAbilityComp->CurrentFlyingState != FlyingState::Ve_OnSlowDown)
	{
		bAimingState = true;
		UGameplayStatics::SetGlobalTimeDilation(this, AimingTimeScale);
		OnAimingStateChange.Broadcast(bAimingState);
	}
	
}

void AINCharacter::Aiming()
{
	//偵測物件設定(保持原先設定，但是更名為Aimable)
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel1);
	//根據相機位置和角度去延伸(可更改)
	FVector CameraPosition = CameraComp->GetComponentLocation();
	FRotator CameraRotation = CameraComp->GetComponentRotation();
	FVector End = CameraPosition+(CameraRotation.Vector() * AimingLength);
	//偵測之球狀物件和擊中初始化
	float Radius = 10.0f;				
	FHitResult Hits;
	FCollisionShape Shape;
	Shape.SetSphere(Radius);
	//看有沒有打到物件
	bool bBlockinghit = GetWorld()->SweepSingleByObjectType(Hits,CameraPosition,End,
		FQuat::Identity,ObjectQueryParams,Shape);
	//根據打到的物件做出反應
	if(bBlockinghit)
	{
		AActor* HitActor = Hits.GetActor();
		if(HitActor)
		{
			if (HitActor->Implements<UINFlyableInterface>()) 
			{
				if(LastHitActor != HitActor)
				{
					if (LastHitActor)
					{
						IINFlyableInterface::Execute_UnAiming(LastHitActor, this);
					}
					LastHitActor = HitActor;
					LastHitComponent = Hits.GetComponent();
					IINFlyableInterface::Execute_BeAiming(LastHitActor, this);
				}
			}
		}
	}
	else
	{
		if (LastHitActor)
		{
			IINFlyableInterface::Execute_UnAiming(LastHitActor, this);
			LastHitActor = nullptr;
			LastHitComponent = nullptr;
		}
	}
}

void AINCharacter::AimEnd()
{
	if (bAimingState == true)
	{
		UGameplayStatics::SetGlobalTimeDilation(this, 1);
		if (LastHitActor != nullptr)
		{
			
			IINFlyableInterface::Execute_UnAiming(LastHitActor, this);
			FlyAbilityComp->Fly(LastHitActor, LastHitComponent, GetChargePercent());
			//可以加一個Delegate，播放飛行過去動畫之類的。
		}
		LastHitActor = nullptr;
		bAimingState = false;
		ChargeTime = 0.0f;
		OnAimingStateChange.Broadcast(bAimingState);
	}	
}



// Called every frame
void AINCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(bAimingState)
	{
		Aiming();
		if(FlyAbilityComp->CurrentFlyCount <FlyAbilityComp-> MaxFlyCount)
		{
			//因為瞄準狀態會導致deltaTime變慢，所以要乘上AimingTimeScale倒數，不然衝能速度也會變慢。
			ChargeTime += DeltaTime * (1/AimingTimeScale);
			if(ChargeTime >= MaxChargeTime)
			{
				ChargeTime = MaxChargeTime;
			}
		}
	}
}



// Called to bind functionality to input
void AINCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis("Moveforward",this, &AINCharacter::Moveforward);
	PlayerInputComponent->BindAxis("MoveRight",this,&AINCharacter::MoveRight);
	PlayerInputComponent->BindAction("Sprint",IE_Pressed,this,&AINCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint",IE_Released,this,&AINCharacter::EndSprint);
	
	PlayerInputComponent->BindAction("Jump",IE_Pressed,this,&AINCharacter::Jump);
	PlayerInputComponent->BindAction("Jump",IE_Released,this,&AINCharacter::SlowDownRelease);
	PlayerInputComponent->BindAction("DropSkill",IE_Pressed,this,&AINCharacter::DropSkill);
	
	PlayerInputComponent->BindAxis("turn",this,&APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp",this,&APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAction("Aiming",IE_Pressed,this,&AINCharacter::AimingStart);
	PlayerInputComponent->BindAction("Aiming",IE_Released,this,&AINCharacter::AimEnd);

}


