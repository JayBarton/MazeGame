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

	void RemoveRandomWalls(TMap<FVector2D, bool>& wallGrid);

	void SetWalls(TMap<FVector2D, bool>& wallGrid);

	void BackTracker(TArray<Cell>& cellGrid, TMap<FVector2D, bool>& wallGrid);

	void SetUpGrids(TArray<Cell>& cellGrid, TMap<FVector2D, bool>& wallGrid);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "Wall")
	TSubclassOf<AActor>  wall = nullptr;

	int minSize = 0;
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	int maxSize = 10;

	//0, 0 point of the grid(X)
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float gridStartX = 450.0f;
	//0, 0 point of the grid(Y)
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float gridStartY = 450.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float wallFloor = 50.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
		float distanceBetweenCells = 100.0f;

};
