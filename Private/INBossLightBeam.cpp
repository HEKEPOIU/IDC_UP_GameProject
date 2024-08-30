// Fill out your copyright notice in the Description page of Project Settings.
#include "INBossLightBeam.h"

#include "AIHelpers.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AINBossLightBeam::AINBossLightBeam()
{
	PrimaryActorTick.bCanEverTick = true;
	//attribute
	AimingTime = 2.0f;
	ShootDelayTime = 0.5f;
	BeamShowUpTime = 1.0f;
	AttachProportion = 0.0f;
	TraceSpeed = 1.0f;
	
	Laser = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Laser"));
	Laser->SetCollisionProfileName("OverlapAllDynamic");
	RootComponent = Laser;
	

}

void AINBossLightBeam::BeginPlay()
{
	Super::BeginPlay();

	CurrentShootState  = EShootState::Ve_Attaching;
	TargetPlayer = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	Laser->SetGenerateOverlapEvents(false);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_NextStageHandle,this,&AINBossLightBeam::WaitForShoot,AimingTime,false);
}

void AINBossLightBeam::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	TryAttachPlayer();
}

void AINBossLightBeam::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Laser->OnComponentBeginOverlap.AddDynamic(this, &AINBossLightBeam::OnOverlapBegin);
}


void AINBossLightBeam::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CurrentShootState != EShootState::Ve_Shoot) return;
	//受傷
	TSubclassOf<UDamageType> DamageTypeClass = UDamageType::StaticClass();
	FDamageEvent DamageEvent(DamageTypeClass);
	OtherActor->TakeDamage(1, DamageEvent, GetInstigatorController(), GetOwner());
	Laser->SetGenerateOverlapEvents(false);
}



FVector AINBossLightBeam::PointToPlayer()
{
	return  (TargetPlayer->GetActorLocation() - GetActorLocation());
}

void AINBossLightBeam::TryAttachPlayer()
{
	if(CurrentShootState != EShootState::Ve_Attaching) return;

	//如果玩家死亡就摧毀，避免空指針bug。
	if (TargetPlayer == nullptr)
	{
		OnDestroy();
	}
	FRotator NewRotation = FRotator(FQuat::Slerp(FQuat(GetActorRotation()),
		FQuat(PointToPlayer().Rotation()), AttachProportion));
	
	//issue::如果PointToPlayer的Y變成0那他會轉一圈 所以可能要放在不會和玩家變成平行的地方
	AttachProportion +=  TraceSpeed * GetWorld()->GetDeltaSeconds();
	SetActorRelativeRotation(NewRotation);
}

void AINBossLightBeam::WaitForShoot()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_NextStageHandle);
	CurrentShootState = EShootState::Ve_Stop;
	LastDirction = GetActorRotation().Vector();
	//準備射擊
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_NextStageHandle,this,&AINBossLightBeam::ShootBeam,ShootDelayTime,false);
}


void AINBossLightBeam::ShootBeam()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_NextStageHandle);
	CurrentShootState = EShootState::Ve_Shoot;
	// Laser->SetGenerateOverlapEvents(true);
	Laser->SetVisibility(false);
	Shoot.Broadcast(LastDirction);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_NextStageHandle,this,&AINBossLightBeam::OnDestroy,BeamShowUpTime,false);
}

void AINBossLightBeam::OnDestroy()
{
	Laser->OnComponentBeginOverlap.RemoveDynamic(this,&AINBossLightBeam::OnOverlapBegin);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_NextStageHandle);
	CurrentShootState = EShootState::Ve_Destroy;
	Destroy();
}










