	// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "INFlyAbilityComponent.generated.h"


class UCharacterMovementComponent;

UENUM(BlueprintType)
enum class FlyingState : uint8 {
	Ve_Normal			UMETA(DisplayName="正常狀態"),
	Ve_OnFlying			UMETA(DisplayName="飛行技能使用途中"),
	Ve_OnFlyStop        UMETA(DisplayName="當擊中目標時"),
	Ve_OnFalling		UMETA(DisplayName="當自然下墜時"),
	Ve_OnSlowDown		UMETA(DisplayName="當使用緩降技能時"),
	Ve_OnDropSkill		UMETA(DisplayName="當使用下墜技能時")
};
//顯示Decal的event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlowDownStateChange,bool,bVisible);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlowDownRemainTimeChange,float,RemainTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHitEnemy);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class IDC_UP_GAMEPROJECT_API UINFlyAbilityComponent: public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UINFlyAbilityComponent();

protected:
	FTimerHandle TimerHandle_FlyingCancel;
	FTimerHandle TimerHandle_Drop;
	FTimerHandle TimerHandle_FlyingRedirect;

	float NowReflectSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Fly_Attribute")
	float FlySpeed;
	UPROPERTY(EditDefaultsOnly, Category = "Fly_Attribute")
	float UnHitDropDelayTime;

	UPROPERTY(EditDefaultsOnly, Category = "Fly_Attribute")
	float FlyingToTargetRedirectRate;

	UPROPERTY(EditDefaultsOnly, Category = "Fly_Attribute")
	float CountSpeedMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = "Fly_Attribute")
	float BaseReflectSpeed;
	
	UPROPERTY(EditDefaultsOnly,Category = "Fly_Attribute", meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float WorldUpEffectRate;

	UPROPERTY(EditDefaultsOnly, Category = "Fly_Attribute")
	float MaxBonusSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Reflect_Attribute")
	float SnapDegreeThreshold;

	UPROPERTY(EditDefaultsOnly, Category = "Reflect_Attribute")
	float AdditionAlphaForDegree;

	UPROPERTY(EditDefaultsOnly, Category="Drop_Attribute")
	float BaseDropSpeed;
	
	UPROPERTY(EditDefaultsOnly, Category="Drop_Attribute")
	float SkillDropTime;
	

	UPROPERTY(EditDefaultsOnly,Category="SlowDown_Attribute")
	float SlowDownAirControl;

	UPROPERTY(EditDefaultsOnly,Category="SlowDown_Attribute")
	float SlowDownGravityScale;

	UPROPERTY(EditDefaultsOnly, Category="SlowDown_Attribute")
	float SlowDownVelocityRemainRate;

	UPROPERTY(EditDefaultsOnly,Category = "SlowDown_Attribute")
	float SlowDownTotalTime;
	
	UPROPERTY(BlueprintReadOnly)
	UCharacterMovementComponent* ChMoveComp;

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	
public:
	//-----slow down timer
	FTimerHandle TimerHandle_SlowDown;
	UPROPERTY(BlueprintReadOnly,Category = "SlowDown_Attribute")
	float SlowDownTime;
	void SlowDownCountDown();
	//-----
	void Fall();
	void Drop();
	void Falling();
	void Fly(AActor* Target, UPrimitiveComponent* TargetLocation, float ChargePercent);

	UPROPERTY(BlueprintAssignable,BlueprintCallable)
	FOnSlowDownStateChange SlowDownStateChange;
	
	UPROPERTY(BlueprintAssignable,BlueprintCallable)
	FOnSlowDownRemainTimeChange SlowDownRemainTimeChange;
	UPROPERTY(BlueprintAssignable,BlueprintCallable)
	FOnHitEnemy OnHitEnemy;
	
	void SlowDown();

	UFUNCTION()
	void HitGround(const FHitResult& Hit);
	
	UFUNCTION(BlueprintCallable)
	void HitEnemy();
	
	UFUNCTION(BlueprintCallable)
	void Reflect(AActor* Actor, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable)
	void FlyForDropSkill();

	UFUNCTION(BlueprintCallable)
	void ChangeCurrentFlyingCount(int Delta);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetRestFlyCountPercent() const { return (MaxFlyCount - CurrentFlyCount) / MaxFlyCount; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetSlowDownTimePercent() const { return SlowDownTime / SlowDownTotalTime; }

	UPROPERTY(BlueprintReadOnly)
	FlyingState CurrentFlyingState = FlyingState::Ve_Normal;
	

	UPROPERTY(EditDefaultsOnly,Category = "Fly_Attribute")
	float MaxFlyCount;

	UPROPERTY(BlueprintReadWrite)
	int CurrentFlyCount;

private:
	AActor* TargetCache;
	UPrimitiveComponent* LastHitComponent;

	float ChargePercentCache;

	void Redirect();
};
