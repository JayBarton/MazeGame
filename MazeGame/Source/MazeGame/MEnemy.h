// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MEnemy.generated.h"

UCLASS()
class MAZEGAME_API AMEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void GetForwardDirection();

	void TurnDirection(float angle);

	//If the enemy should check to turn before reaching a wall
	bool checkRight = true;
	bool checkLeft = true;

	bool tryTurn = false;
	int turnRight = 0;
	int turnLeft = 0;
	bool turned = false;

	//will expand into behavior states later
	bool moveToPosition = false;

	FVector2D positionToMove;
};
