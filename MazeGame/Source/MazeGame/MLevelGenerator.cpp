// Fill out your copyright notice in the Description page of Project Settings.


#include "MLevelGenerator.h"
#include "Math/UnrealMathUtility.h" 

// Sets default values
AMLevelGenerator::AMLevelGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMLevelGenerator::BeginPlay()
{
	Super::BeginPlay();
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	grid.Init(Cell(), maxSize * maxSize);
	wallPositions.Reserve(100);

	//Temp until I think of a way to get the current index better
	int count = 0;
	for (int i = minSize; i < maxSize; i++)
	{
		for (int c = minSize; c < maxSize; c++)
		{
			FVector location(450.0f - (i * 100), 450.0f - (c * 100), 50.0f);
			if (i % 2 == 0 && c % 2 == 0)
			{
				grid[count].position = FVector2D(i, c);

				//Set grid neighbours
				grid[count].neighbours.Reserve(4);
				FVector2D position = grid[count].position;
				if (position.Y - 2 >= minSize)
				{
					grid[count].neighbours.Add(&grid[count - 2]);
				}
				if (position.Y + 2 < maxSize)
				{
					grid[count].neighbours.Add(&grid[count + 2]);
				}
				if (position.X - 2 >= minSize)
				{
					grid[count].neighbours.Add(&grid[count - 2 * maxSize]);
				}
				if (position.X + 2 < maxSize)
				{
					grid[count].neighbours.Add(&grid[count + 2 * maxSize]);
				}
			}

			else if (i % 2 != 0 || c % 2 != 0)
			{
				//walls
				grid[count].wall = true;
				grid[count].position = FVector2D(location.X, location.Y);
			}
			count++;
		}
	}

	TArray<Cell*> backTracker;
	backTracker.Push(&grid[0]);
	backTracker[0]->checked = true;

	while (backTracker.Num() > 0)
	{
		Cell* currentCell = backTracker.Pop();
		//Remove any neighbours that have already been checked
		for (int i = 0; i < currentCell->neighbours.Num(); i++)
		{
			if (currentCell->neighbours[i]->checked)
			{
				currentCell->neighbours.RemoveAt(i);
				i--;
			}
		}
		int numberOfNeighbors = currentCell->neighbours.Num();
		if (numberOfNeighbors > 0)
		{
			backTracker.Push(currentCell);
			int32 neighbor = FMath::RandRange(0, numberOfNeighbors - 1);
			Cell* neighborCell = currentCell->neighbours[neighbor];

			//remove wall between
			FVector2D p(currentCell->position.X - neighborCell->position.X, currentCell->position.Y - neighborCell->position.Y);
			p /= 2;
			FVector2D idkman = currentCell->position - p;
			int32 index = (idkman.X * maxSize) + idkman.Y;

			grid[index].wall = false;

			neighborCell->checked = true;
			backTracker.Push(neighborCell);
		}
	}

	for (int i = 0; i < grid.Num(); i++)
	{
		if (grid[i].wall)
		{
			FVector location(grid[i].position.X, grid[i].position.Y, 50.0f);
			wallPositions.Add(location);
		}
	}

	for (int i = 0; i < wallPositions.Num(); i++)
	{
		AActor* newWall = GetWorld()->SpawnActor<AActor>(wall, wallPositions[i], FRotator::ZeroRotator, SpawnParams);

	}
	
}

// Called every frame
void AMLevelGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

