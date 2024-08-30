// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\INFlyAbilityComponent.h"

#include "CoreMinimal.h"
#include "INFlyableInterface.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameSession.h"


// Sets default values for this component's properties
UINFlyAbilityComponent::UINFlyAbilityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	SlowDownTotalTime = 1.5f;
	SlowDownTime = SlowDownTotalTime;
	BaseReflectSpeed = 2000;
	MaxBonusSpeed = 2000;
	FlySpeed = 5000;
	CountSpeedMultiplier = 300;
	WorldUpEffectRate = 0.55f;
	MaxFlyCount = 3;
	UnHitDropDelayTime = 1.0f;
	FlyingToTargetRedirectRate = 0.1f;
	//下降速度
	SkillDropTime = 1.0f;
	BaseDropSpeed = 300;
	//緩降控制
	SlowDownAirControl = 0.8f;
	SlowDownGravityScale = 0.5f;

	SnapDegreeThreshold = 90;
	AdditionAlphaForDegree = 0.2f;

	SlowDownVelocityRemainRate = 0.33f;

}

void UINFlyAbilityComponent::HitEnemy()
{
	//開啟移動輸入。
	GetOwner()->GetInstigatorController()->SetIgnoreMoveInput(false);
	
	//這裡主要是之後可以用來放當擊中物體時的一些特效之類的東西
	if (CurrentFlyingState == FlyingState::Ve_OnDropSkill )
	{
		ChangeCurrentFlyingCount(-2);
	}
	OnHitEnemy.Broadcast();
	CurrentFlyingState = FlyingState::Ve_OnFlyStop;
	GetOwner()->GetWorldTimerManager().ClearTimer(TimerHandle_FlyingCancel);
	GetOwner()->GetWorldTimerManager().ClearTimer(TimerHandle_FlyingRedirect);
}

void UINFlyAbilityComponent::Fly(AActor* Target, UPrimitiveComponent* TargetPrimitiveComponent, float ChargePercent)	
{
	if(CurrentFlyCount < MaxFlyCount )
	{
		//取消移動輸入。
		GetOwner()->GetInstigatorController()->SetIgnoreMoveInput(true);
		//要先讓先停下來。不然她速度會跌加上去，會很容易導致打不中目標。
		ChMoveComp->StopMovementImmediately();
		// FVector VectorToTarget = (Target->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal();
		// ChMoveComp->AddImpulse(VectorToTarget * (FlySpeed + (ChargePercent* MaxBonusSpeed)), true);
		//這是為了不受到地面阻力影響，所以要設定它內建的FlyMode。
		TargetCache = Target;
		LastHitComponent = TargetPrimitiveComponent;
		ChargePercentCache = ChargePercent;
		ChMoveComp->SetMovementMode(MOVE_Flying);
		//避免沒撞到人一直飛。
		GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle_FlyingCancel, this,
			&UINFlyAbilityComponent::Fall, UnHitDropDelayTime);
		GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle_FlyingRedirect, this,
			&UINFlyAbilityComponent::Redirect, FlyingToTargetRedirectRate, true);
		CurrentFlyingState = FlyingState::Ve_OnFlying;
		
	}
}


void UINFlyAbilityComponent::Reflect(AActor* Actor, const FHitResult& Hit)
{
	FVector Normal = Hit.ImpactNormal;
	FVector WorldUp = FVector::UpVector;
	FVector ReflectVector =  FMath::GetReflectionVector(ChMoveComp->Velocity,Normal).GetSafeNormal();
	//算出世界UP座標與反射角的夾角，來辨別要不要讓速度吸附到世界上方。
	float AngleInDegrees = FMath::RadiansToDegrees(FMath::Acos(WorldUp.Dot(ReflectVector)));
	// UE_LOG(LogTemp, Warning, TEXT("AngleInDegrees: %f"), AngleInDegrees);
	float AdditionAlpha = 0.0f;
	//代表如果角色是從上面撞到木鰾，就要往上吸附一點，不然他很難彈高。
	if (AngleInDegrees >= SnapDegreeThreshold)
	{
		AdditionAlpha += AdditionAlphaForDegree;
	}
	float Alpha = FMath::Min(WorldUpEffectRate + AdditionAlpha, 1.0f);
	//因為我想要讓他往上反彈又要保有原先的撞擊角度，所以用Lerp去做插值。
	FVector InterpolatedVector = FMath::Lerp(ReflectVector, WorldUp, Alpha);
	ChMoveComp->Velocity = InterpolatedVector.GetSafeNormal() * BaseReflectSpeed
	+ (CurrentFlyCount * CountSpeedMultiplier);
	//要設定回掉落狀態，如果保持在Flying狀態會導致無法下墜。

	ChMoveComp->SetMovementMode(MOVE_Falling);
	CurrentFlyingState = FlyingState::Ve_OnFalling;

	if(	Actor->Implements<UINFlyableInterface>())
	{
		IINFlyableInterface::Execute_HitReact(Actor,Cast<APawn>(GetOwner()));
	}

}



void UINFlyAbilityComponent::FlyForDropSkill()
{
	CurrentFlyingState = FlyingState::Ve_OnFalling;
	ChMoveComp->SetMovementMode(MOVE_Falling);
	ChMoveComp->Velocity = FVector::UpVector * (BaseReflectSpeed);
}

void UINFlyAbilityComponent::ChangeCurrentFlyingCount(int Delta)
{
	CurrentFlyCount =  FMath::Clamp(CurrentFlyCount+Delta,0,MaxFlyCount);
}


void UINFlyAbilityComponent::Redirect()
{
	if (TargetCache)
	{
		ChMoveComp->StopMovementKeepPathing();
		FVector VectorToTarget = (LastHitComponent->GetComponentLocation() - GetOwner()->GetActorLocation()).GetSafeNormal();
		GetOwner()->SetActorRotation((LastHitComponent->GetComponentLocation() - GetOwner()->GetActorLocation()).Rotation());
		ChMoveComp->Velocity = VectorToTarget * (FlySpeed + (ChargePercentCache* MaxBonusSpeed));
	}
}

void UINFlyAbilityComponent::BeginPlay()
{
	Super::BeginPlay();
	ChMoveComp = GetOwner()->FindComponentByClass<UCharacterMovementComponent>();
	ensureMsgf(ChMoveComp, TEXT("本角色沒有CharacterMovementComponent，FlyAbilityComponent無法運作"));
	
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (ensureMsgf(Character, TEXT("本角色只能用在Character上，FlyAbilityComponent無法運作")))
	{
		Character->LandedDelegate.AddDynamic(this, &UINFlyAbilityComponent::HitGround);
	}
}

void UINFlyAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//當技能使用時要讓他往下持續加一個下落力量讓他有加速度感，這個速度會比正常下落的加速度還快
	if (CurrentFlyingState == FlyingState::Ve_OnDropSkill || CurrentFlyingState == FlyingState::Ve_OnFalling )
	{
		FVector DropForce = FVector::DownVector * BaseDropSpeed;
		
		float SkillDropSpeedMultiplier = GetOwner()->GetActorLocation().Z / SkillDropTime;
		CurrentFlyingState == FlyingState::Ve_OnDropSkill ? DropForce *= SkillDropSpeedMultiplier: DropForce;
		ChMoveComp->AddForce(DropForce);
	}
	
}

void UINFlyAbilityComponent::Falling()//切換成墜落模式
{
	if (CurrentFlyingState == FlyingState::Ve_OnSlowDown)
	{
		SlowDownStateChange.Broadcast(false);
	}
	//切換回正常墜落數值
	ChMoveComp->AirControl = 0.2f;
	ChMoveComp->GravityScale = 2.5f;
	CurrentFlyingState = FlyingState::Ve_OnFalling;
}

void UINFlyAbilityComponent::Fall()//掉到地板
{
	ChMoveComp->AirControl = 0.2f;
	ChMoveComp->GravityScale = 2.5f;
	
	GetOwner()->GetWorldTimerManager().ClearTimer(TimerHandle_FlyingRedirect);
	ChMoveComp->SetMovementMode(MOVE_Falling);
	CurrentFlyingState = FlyingState::Ve_Normal;
}

void UINFlyAbilityComponent::Drop()
{
	//因為要先切換成Flying才能停下來。
	ChMoveComp->SetMovementMode(MOVE_Flying);
	ChMoveComp->StopMovementImmediately();
	ChMoveComp->SetMovementMode(MOVE_Falling);
	//切換回正常墜落數值
	ChMoveComp->AirControl = 0.2f;
	ChMoveComp->GravityScale = 2.5f;
	CurrentFlyingState = FlyingState::Ve_OnDropSkill;
	SlowDownStateChange.Broadcast(false);
	//重設計時器
	SlowDownTime = SlowDownTotalTime;

}

void UINFlyAbilityComponent::SlowDown()
{
		//開啟範圍顯示
		SlowDownStateChange.Broadcast(true);
		//去除原本降落的力
		ChMoveComp->SetMovementMode(MOVE_Flying);
		ChMoveComp->Velocity *= SlowDownVelocityRemainRate;
		ChMoveComp->SetMovementMode(MOVE_Falling);
		//設定控制變數
		ChMoveComp->AirControl = SlowDownAirControl;
		ChMoveComp->GravityScale = SlowDownGravityScale;
		CurrentFlyingState = FlyingState::Ve_OnSlowDown;
}

void UINFlyAbilityComponent::SlowDownCountDown()
{
	//這甚麼奇怪得遞歸，為啥不直接啟動一個bool然後在loop裡面減少，這樣變成他一定要0.1秒才扣一次誒 。
	if(SlowDownTime>0.0f && ChMoveComp->MovementMode == MOVE_Falling)
	{
		GetOwner()->GetWorld()->GetTimerManager().SetTimer(TimerHandle_SlowDown, this,
			&UINFlyAbilityComponent::SlowDownCountDown, 0.1f,false);
		SlowDownTime-= 0.1f;
		SlowDownRemainTimeChange.Broadcast(SlowDownTime);
		// UE_LOG(LogTemp, Log, TEXT("剩餘時間 %f"),SlowDownTime);
		if(CurrentFlyingState == FlyingState::Ve_OnFalling)
		{
			SlowDown();
		}
	}else{
		CurrentFlyingState == FlyingState::Ve_OnSlowDown ? Falling() : Fall();
		GetOwner()->GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SlowDown);
	}
}



void UINFlyAbilityComponent::HitGround(const FHitResult& Hit)
{
	Falling();
	//重設計時器
	SlowDownTime = SlowDownTotalTime;
	//Rest FlyCount
	CurrentFlyCount = 0;
	CurrentFlyingState = FlyingState::Ve_Normal;
	//開啟移動輸入。
	GetOwner()->GetInstigatorController()->SetIgnoreMoveInput(false);
}






