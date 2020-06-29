// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h" 
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MLevelGenerator.generated.h"


struct Cell
{
	bool checked = false;
	bool wall = false; //hopefully temp until i figure out a better solution
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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TArray<Cell> grid;
	TArray<FVector> wallPositions;

	UPROPERTY(EditDefaultsOnly, Category = "Wall")
	TSubclassOf<AActor>  wall = nullptr;

	int minSize = 0;
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
		int maxSize = 11;
};
