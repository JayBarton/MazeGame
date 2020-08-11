// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MazeGameGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class MAZEGAME_API AMazeGameGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	void CompleteLevel(bool success, APawn* InstigatorPawn = nullptr);

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void OnLevelCompleted(bool success);
	
	UPROPERTY(EditDefaultsOnly, Category = "Pointless")
	TSubclassOf<AActor>  view = nullptr;
};
