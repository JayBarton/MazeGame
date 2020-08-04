// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MAZEGAME_API UMGameInstance : public UGameInstance
{
	GENERATED_BODY()
	

public:
	UPROPERTY(BlueprintReadWrite, Category = "Player")

	int mazeType = 0;
};
