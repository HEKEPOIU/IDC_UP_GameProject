// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "INCharacter.generated.h"

class UINFlyAbilityComponent;
class UCameraComponent;
class USpringArmComponent;



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAimingStateChange, bool, bAiming);
UCLASS()
class IDC_UP_GAMEPROJECT_API AINCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AINCharacter();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsHitTarget() const { return LastHitActor != nullptr; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetChargePercent() const { return ChargeTime / MaxChargeTime; }

protected:
	//TODO: 增加豆子彈跳點的技能。 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USpringArmComponent *SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent *CameraComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UINFlyAbilityComponent* FlyAbilityComp;


	// ----------------- Movement -----------------
	void Moveforward(float Value);
	void MoveRight(float Value);
	virtual void Jump() override;
	void SlowDownRelease();
	void StartSprint();
	void EndSprint();
	void DropSkill();


	UPROPERTY(EditDefaultsOnly, Category = "Movement_Attribute")
	float SprintAdditionalSpeed;

	// ----------------- Aimming Ability(Can Be Move To Components) -----------------
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	bool bAimingState;
	UPROPERTY(BlueprintAssignable)
	FOnAimingStateChange OnAimingStateChange;
	UPROPERTY(EditDefaultsOnly, Category = "Aiming_Attribute")
	float AimingLength;

	UPROPERTY(BlueprintReadOnly, Category = "Aiming_Attribute")
	float ChargeTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aiming_Attribute")
	float MaxChargeTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aiming_Attribute")
	float AimingTimeScale;
	

	void AimingStart();
	void Aiming();
	void AimEnd();
	
	AActor* LastHitActor;
	UPrimitiveComponent* LastHitComponent;
	bool bIsRunning;
};