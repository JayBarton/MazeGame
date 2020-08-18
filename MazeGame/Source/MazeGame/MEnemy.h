// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MEnemy.generated.h"

class UPawnSensingComponent;
class AMPlayerCharacter;

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

	void Attack();

	void WanderAroundPlayer(FVector2D& playerPosition);

	void MoveBackToPlayer(FVector2D& currentPosition, FVector2D& playerPosition);

	void Wander(FVector2D& currentPosition, FVector2D& playerPosition);

	void FollowPlayer(FVector2D& playerPosition);

	void Reset(FVector2D& currentPosition);

	void GetForwardDirection();

	void CheckSides(bool& turned);

	void CheckFront();

	void TurnDirection(float angle);

	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);

	UFUNCTION()
	void OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volumne);

	void FoundPlayer();

	UPROPERTY(VisibleAnywhere, Category = "AI")
	UPawnSensingComponent* PawnSensingComp;

	//If the enemy should check to turn before reaching a wall
	bool checkRight = true;
	bool checkLeft = true;

	bool tryTurn = false;
	int turnRight = 0;
	int turnLeft = 0;

	//will expand into behavior states later
	bool moveToPosition = false;

	bool foundPlayer = false;
	UPROPERTY(BlueprintReadonly, Category = "Attack")
	bool attacking = false;
	UPROPERTY(BlueprintReadonly, Category = "Attack")
	bool victory = false;

	bool canSee = false;
	bool attacked = false;

	float seenTimer = 0.0f;
	float heardTimer = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float attackDistance = 150.0f;
	UPROPERTY(EditAnywhere, Category = "Attack")
	float attackRange = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float attackLength = 1.7f;
	float halfAttackLength = 0.0f;

	float attackTime = 0.0f;

	AMPlayerCharacter* thePlayer;


};
