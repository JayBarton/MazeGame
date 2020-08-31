// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h" 
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MLevelGenerator.generated.h"


struct Cell
{
	bool checked = false;
	FVector2D position;
	TArray<Cell*> neighbours;
};

UCLASS()
class MAZEGAME_API AMLevelGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMLevelGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Prim(TArray<Cell>& cellGrid, TMap<FVector2D, bool>& wallGrid);

	void RemoveRandomWalls(TMap<FVector2D, bool>& wallGrid);

	void SetWalls(TArray<Cell>& cellGrid, TMap<FVector2D, bool>& wallGrid);

	void SpawnLights(TArray<Cell>& cellGrid, float startPosition, TMap<FVector2D, bool>& wallGrid, FActorSpawnParameters& SpawnParams);

	void SpawnOuterWalls(float startPosition, FActorSpawnParameters& SpawnParams);

	void SpawnTreasure(FActorSpawnParameters& SpawnParams, TArray<Cell>& cellGrid, float startPosition);

	void SpawnPlayer(float startPosition, FActorSpawnParameters& SpawnParams);

	void BackTracker(TArray<Cell>& cellGrid, TMap<FVector2D, bool>& wallGrid);

	void SetUpGrids(TArray<Cell>& cellGrid, TMap<FVector2D, bool>& wallGrid);


public:	

	UPROPERTY(EditDefaultsOnly, Category = "Maze")
	TSubclassOf<AActor>  wall = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Maze")
	TSubclassOf<AActor>  outerWall = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Maze")
	TSubclassOf<AActor>  floor = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Maze")
	TSubclassOf<AActor> entrance = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Maze")
	TSubclassOf<AActor> enemy = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Maze")
	TSubclassOf<AActor> treasure = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Maze")
	TSubclassOf<AActor> light = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Maze")
	TSubclassOf<AActor>  thePlayer = nullptr;

	int minSize = 0;
	UPROPERTY(EditAnywhere, Category = "Setup")
	int maxSize = 10;

	int mazeType;

	int treasureLocation;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float wallFloor = 50.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float distanceBetweenCells = 100.0f;

};
