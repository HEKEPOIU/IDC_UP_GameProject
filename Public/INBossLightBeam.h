// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystemComponent.h"
#include "INBossLightBeam.generated.h"

class UStaticMeshComponent;
class UCapsuleComponent;

UENUM(BlueprintType)
enum class EShootState : uint8 {
	Ve_Attaching		UMETA(DisplayName="接近玩家"),
	Ve_Stop				UMETA(DisplayName="停下準備射擊"),
	Ve_Shoot			UMETA(DisplayName="射擊"),
	Ve_Destroy			UMETA(DisplayName="摧毀"),
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FShoot, FVector, LastDirction);
UCLASS()
class IDC_UP_GAMEPROJECT_API AINBossLightBeam : public AActor
{
	GENERATED_BODY()
private:
	TWeakObjectPtr<AActor> TargetPlayer;
	FVector LastDirction;
protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UStaticMeshComponent* Laser;

	FTimerHandle TimerHandle_NextStageHandle;

	UPROPERTY(BlueprintReadOnly,BlueprintAssignable)
	FShoot Shoot;
	
	UPROPERTY(EditDefaultsOnly, Category="TimeAttribute")
	float AimingTime;
	UPROPERTY(EditDefaultsOnly, Category="TimeAttribute")
	float ShootDelayTime;
	UPROPERTY(EditDefaultsOnly, Category="TimeAttribute")
	float BeamShowUpTime;
	UPROPERTY(EditDefaultsOnly, Category="TraceSpeed")
	float TraceSpeed;
	float AttachProportion;
	

	UPROPERTY(BlueprintReadOnly)
	EShootState CurrentShootState;
	
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
		class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void PostInitializeComponents() override;

	
	
	FVector PointToPlayer();
	void TryAttachPlayer();
	void WaitForShoot();
	void ShootBeam();
	void OnDestroy();
public:	
	// Sets default values for this actor's properties
	AINBossLightBeam();
};
