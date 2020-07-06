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

	TArray<Cell> cellGrid;
	TMap<FVector2D, bool> wallGrid;
	cellGrid.Init(Cell(), (maxSize * maxSize) * 0.25f);
	SetUpGrids(cellGrid, wallGrid);

	//Prim(cellGrid, wallGrid);

	BackTracker(cellGrid, wallGrid);

	RemoveRandomWalls(wallGrid);

	SetWalls(wallGrid);
}

void AMLevelGenerator::RemoveRandomWalls(TMap<FVector2D, bool>& wallGrid)
{
	TArray<FVector2D> wallIndexes;
	wallGrid.GenerateKeyArray(wallIndexes);

	for (int i = 0; i < maxSize * 0.5f; i++)
	{
		int randomIndex = FMath::RandRange(0, wallIndexes.Num() - 1);

		FVector2D currentWall = wallIndexes[randomIndex];

		FVector2D right(currentWall.X + 1, currentWall.Y);
		FVector2D left(currentWall.X - 1, currentWall.Y);
		FVector2D up(currentWall.X, currentWall.Y - 1);
		FVector2D down(currentWall.X, currentWall.Y + 1);
		if ((!wallGrid.Contains(right) && !wallGrid.Contains(left)) && (wallGrid.Contains(up) && wallGrid.Contains(down)))
		{
			wallGrid.Remove(currentWall);
			wallIndexes.RemoveAt(randomIndex);
		}
		else if ((!wallGrid.Contains(up) && !wallGrid.Contains(down)) && (wallGrid.Contains(right) && wallGrid.Contains(left)))
		{
			wallGrid.Remove(currentWall);
			wallIndexes.RemoveAt(randomIndex);
		}
		else
		{
			i--;
		}
	}
}

void AMLevelGenerator::SetUpGrids(TArray<Cell>& cellGrid, TMap<FVector2D, bool>& wallGrid)
{
	//Temp until I think of a way to get the current index better
	int count = 0;
	for (int i = minSize; i < maxSize; i += 2)
	{
		for (int c = minSize; c < maxSize; c += 2)
		{
			cellGrid[count].position = FVector2D(i, c);

			//Set grid neighbours
			cellGrid[count].neighbours.Reserve(4);
			FVector2D position = cellGrid[count].position;
			if (position.Y - 2 >= minSize)
			{
				cellGrid[count].neighbours.Add(&cellGrid[count - 1]);
			}
			if (position.Y + 2 < maxSize)
			{
				cellGrid[count].neighbours.Add(&cellGrid[count + 1]);
			}
			if (position.X - 2 >= minSize)
			{
				cellGrid[count].neighbours.Add(&cellGrid[count - 1 * maxSize * 0.5f]);
			}
			if (position.X + 2 < maxSize)
			{
				cellGrid[count].neighbours.Add(&cellGrid[count + 1 * maxSize * 0.5f]);
			}
			count++;
		}
	}


	for (int i = minSize; i < maxSize - 1; i++)
	{
		for (int c = minSize; c < maxSize - 1; c++)
		{
			if (i % 2 != 0 || c % 2 != 0)
			{
				wallGrid.Add(FVector2D(i, c), true);
			}
		}
	}
}

void AMLevelGenerator::BackTracker(TArray<Cell>& cellGrid, TMap<FVector2D, bool>& wallGrid)
{
	TArray<Cell*> backTracker;
	//Generate maze from the center
	backTracker.Push(&cellGrid[cellGrid.Num() * 0.5f]);
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
			FVector2D betweenPoint = currentCell->position - (p * 0.5f);

			wallGrid.Remove(betweenPoint);

			neighborCell->checked = true;
			backTracker.Push(neighborCell);
		}
	}
}

void AMLevelGenerator::Prim(TArray<Cell>& cellGrid, TMap<FVector2D, bool>& wallGrid)
{
	Cell* currentCell;
	//Generate maze from the center
	//this is not the center
	currentCell = &cellGrid[0];
	currentCell->checked = true;
	TArray<Cell*> frontier;

	for (int i = 0; i < currentCell->neighbours.Num(); i++)
	{
		frontier.Add(currentCell->neighbours[i]);
	}
	while (frontier.Num() > 0)
	{
		int index = FMath::RandRange(0, frontier.Num() - 1);
		currentCell = frontier[index];
		currentCell->checked = true;

		frontier.RemoveAt(index);
		TArray<Cell*>neighboursInMaze;
		for (int i = 0; i < currentCell->neighbours.Num(); i++)
		{
			if (currentCell->neighbours[i]->checked)
			{
				neighboursInMaze.Add(currentCell->neighbours[i]);
			}
		}
		int neighbourIndex = FMath::RandRange(0, neighboursInMaze.Num() - 1);

		Cell* neighborCell = neighboursInMaze[neighbourIndex];

		//remove wall between
		FVector2D p(currentCell->position.X - neighborCell->position.X, currentCell->position.Y - neighborCell->position.Y);
		FVector2D betweenPoint = currentCell->position - (p * 0.5f);

		wallGrid.Remove(betweenPoint);

		for (int i = 0; i < currentCell->neighbours.Num(); i++)
		{
			if (!currentCell->neighbours[i]->checked)
			{
				//Maybe there is a better solution than add unique?
				frontier.AddUnique(currentCell->neighbours[i]);
			}
		}
	}
}


void AMLevelGenerator::SetWalls(TMap<FVector2D, bool>& wallGrid)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	for (auto& cell : wallGrid)
	{
		FVector2D p = cell.Key;
		FVector location(gridStartX - (p.X * distanceBetweenCells), gridStartY - (p.Y * distanceBetweenCells), wallFloor);
		AActor* newWall = GetWorld()->SpawnActor<AActor>(wall, location, FRotator::ZeroRotator, SpawnParams);
	}
	//Outer walls
	//Will need to rework this at some point so that I can create an entrance

	//Original
	for (int i = 0; i < maxSize; i++)
	{
		FVector location(gridStartX - (i * distanceBetweenCells), gridStartY + distanceBetweenCells, wallFloor);
		GetWorld()->SpawnActor<AActor>(wall, location, FRotator::ZeroRotator, SpawnParams);

		location = FVector((gridStartX + distanceBetweenCells) - (i * distanceBetweenCells), gridStartY - (distanceBetweenCells * (maxSize - 1)), wallFloor);
		GetWorld()->SpawnActor<AActor>(wall, location, FRotator::ZeroRotator, SpawnParams);

		location = FVector(gridStartX - (distanceBetweenCells * (maxSize - 1)), gridStartY - (i * distanceBetweenCells), wallFloor);
		GetWorld()->SpawnActor<AActor>(wall, location, FRotator::ZeroRotator, SpawnParams);

		location = FVector(gridStartX + distanceBetweenCells, gridStartY + distanceBetweenCells - (i * distanceBetweenCells), wallFloor);
		GetWorld()->SpawnActor<AActor>(wall, location, FRotator::ZeroRotator, SpawnParams);
	}
}



// Called every frame
void AMLevelGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

