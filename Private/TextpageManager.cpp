// Fill out your copyright notice in the Description page of Project Settings.
#include "..\Public\TextpageManager.h"

// Sets default values
ATextpageManager::ATextpageManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATextpageManager::BeginPlay()
{
	Super::BeginPlay();
	
}

TArray<FString> ATextpageManager::TextFromFile(const FString FilePath)
{
	//todo:: Build 之後更改路徑位置
	//取得content資料夾相對路徑
	FString AbsolutePath = FPaths::ProjectContentDir() + FilePath;
	
	TArray<FString> OutputStrings ;
	OutputStrings.Empty();
	//確認檔案存在
	if(!FPlatformFileManager::Get().GetPlatformFile().FileExists(* AbsolutePath ))
	{
		UE_LOG(LogTemp,Warning,TEXT("File doesn't exist"))
		return OutputStrings ;
	}
	
	//嘗試讀黨
	//用enter來分句子
	if(!FFileHelper::LoadANSITextFileToStrings(*AbsolutePath ,&IFileManager::Get(),OutputStrings))
	{
		UE_LOG(LogTemp,Warning,TEXT("Read File faild"))
		return OutputStrings ;
	}

	UE_LOG(LogTemp,Log,TEXT("Read File Success"))
	return OutputStrings;
}







