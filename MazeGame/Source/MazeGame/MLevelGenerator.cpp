// Fill out your copyright notice in the Description page of Project Settings.


#include "MLevelGenerator.h"
#include "Math/UnrealMathUtility.h" 
#include "Kismet/GameplayStatics.h" 
#include "Engine/World.h" 
#include "MGameInstance.h"
#include "GameFramework/Character.h"


#include "GameFramework/PlayerStart.h" 
#include "MPlayerCharacter.h"

// Sets default values
AMLevelGenerator::AMLevelGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AMLevelGenerator::BeginPlay()
{
	Super::BeginPlay();

	if (auto instance = Cast<UMGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		int type = instance->mazeType;
		if (type >= 0)
		{
			mazeType = instance->mazeType;
		}
		else
		{
			mazeType = FMath::RandRange(0, 1);
		}
		treasureLocation = instance->treasureLocation;
	}
	else
	{
		mazeType = 0;
		treasureLocation = 0;
	}

	TArray<Cell> cellGrid;
	TMap<FVector2D, bool> wallGrid;
	cellGrid.Init(Cell(), (maxSize * maxSize) * 0.25f);
	SetUpGrids(cellGrid, wallGrid);

	if (mazeType == 0)
	{
		BackTracker(cellGrid, wallGrid);
	}
	else
	{
		Prim(cellGrid, wallGrid);
	}
	RemoveRandomWalls(wallGrid);

	SetWalls(cellGrid, wallGrid);
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


			//if (i == 8 && c == 8)
			{
				/*cellGrid[count].neighbours.Add(&cellGrid[count - 2]);
				cellGrid[count].neighbours.Add(&cellGrid[count + 2]);
				cellGrid[count].neighbours.Add(&cellGrid[count - 2 * maxSize * 0.5f]);
				cellGrid[count].neighbours.Add(&cellGrid[count + 2 * maxSize * 0.5f]);*/
			}
		//	else
			{
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
			}
			count++;
		}
	}

	//Abandoned idea, may revist later
/*	count = 0;
	for (int i = minSize; i < maxSize; i += 2)
	{
		for (int c = minSize; c < maxSize; c += 2)
		{
			//8,8 is the center with a size of 20
			if (count == 44)
			{
				UE_LOG(LogTemp, Warning, TEXT("%i, %i"), i, c);
				UE_LOG(LogTemp, Warning, TEXT("%i"), count - 2);
				UE_LOG(LogTemp, Warning, TEXT("%i"), count + 2);
				UE_LOG(LogTemp, Warning, TEXT("%f"), count - 2 * maxSize * 0.5f);
				UE_LOG(LogTemp, Warning, TEXT("%f"), count + 2 * maxSize * 0.5f);

				for (int q = 0; q < cellGrid[count].neighbours.Num(); q++)
				{
					float x = cellGrid[count].neighbours[q]->position.X;
					float y = cellGrid[count].neighbours[q]->position.Y;
					UE_LOG(LogTemp, Warning, TEXT("%f, %f"), x, y);
					float startPosition = (maxSize - 2) * distanceBetweenCells * 0.5f;
					FVector2D p(x, y);
					FVector location(startPosition - (p.X * distanceBetweenCells), startPosition - (p.Y * distanceBetweenCells), wallFloor);
				}
			}
			count++;
		}
	}*/

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
	backTracker.Push(&cellGrid[0]);
	backTracker[0]->checked = true;
	//cellGrid[44].checked = true;
	//UE_LOG(LogTemp, Warning, TEXT("%i, %f, %f"), maxSize * maxSize, backTracker[0]->position.X, backTracker[0]->position.Y);
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

void AMLevelGenerator::SetWalls(TArray<Cell>& cellGrid, TMap<FVector2D, bool>& wallGrid)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	float startPosition = (maxSize - 2) * distanceBetweenCells * 0.5f;
	for (auto& cell : wallGrid)
	{
		FVector2D p = cell.Key;
		FVector location(startPosition - (p.X * distanceBetweenCells), startPosition - (p.Y * distanceBetweenCells), wallFloor);
		AActor* newWall = GetWorld()->SpawnActor<AActor>(wall, location, FRotator::ZeroRotator, SpawnParams);
	}

	SpawnLights(cellGrid, startPosition, wallGrid, SpawnParams);
	
	SpawnOuterWalls(startPosition, SpawnParams);

	//Floor
	AActor* theFloor = GetWorld()->SpawnActor<AActor>(floor, FVector(0, 0, 0), FRotator::ZeroRotator, SpawnParams);
	float floorSize = maxSize / 5;
	theFloor->SetActorScale3D(FVector(floorSize, floorSize, 1.0f));

	//Ceiling
	AActor* theCeiling = GetWorld()->SpawnActor<AActor>(floor, FVector(0, 0, 350.0f), FRotator::ZeroRotator, SpawnParams);
	theCeiling->SetActorScale3D(FVector(floorSize, floorSize, 1.0f));

	//Entrance
	float entrancePosition = startPosition;
	GetWorld()->SpawnActor<AActor>(entrance, FVector(entrancePosition, entrancePosition, 0), FRotator::ZeroRotator, SpawnParams);

	SpawnTreasure(SpawnParams, cellGrid, startPosition);

	SpawnPlayer(startPosition, SpawnParams);

}

void AMLevelGenerator::SpawnLights(TArray<Cell>& cellGrid, float startPosition, TMap<FVector2D, bool>& wallGrid, FActorSpawnParameters& SpawnParams)
{
	int side = 0;
	for (int i = 2; i < cellGrid.Num(); i += 2)
	{
		FVector2D p = cellGrid[i].position;
		FVector location(startPosition - (p.X * distanceBetweenCells), startPosition - (p.Y * distanceBetweenCells), 250);
		FRotator rotation = FRotator::ZeroRotator;
		if (side == 0)
		{
			if (wallGrid.Contains(FVector2D(p.X, p.Y + 1)) || p.Y >= maxSize)
			{
				location.Y -= 100;
				rotation = FRotator(0.0f, 0.0f, 180.0f);
				AActor* newLight = GetWorld()->SpawnActor<AActor>(light, location, rotation, SpawnParams);
				side = 1;
			}
			else if (wallGrid.Contains(FVector2D(p.X + 1, p.Y)) || p.X >= maxSize)
			{
				location.X -= 100;
				rotation = FRotator(0.0f, 90.0f, 0.0f);
				AActor* newLight = GetWorld()->SpawnActor<AActor>(light, location, rotation, SpawnParams);
				//	side = 1;
			}
		}
		else if (side == 1)
		{
			if (wallGrid.Contains(FVector2D(p.X, p.Y - 1)) || p.Y <= 0)
			{
				location.Y += 100;
				AActor* newLight = GetWorld()->SpawnActor<AActor>(light, location, rotation, SpawnParams);
				side = 0;
			}
			else if (wallGrid.Contains(FVector2D(p.X - 1, p.Y)) || p.X <= 0)
			{
				location.X += 100;
				rotation = FRotator(0.0f, -90.0f, 0.0f);
				AActor* newLight = GetWorld()->SpawnActor<AActor>(light, location, rotation, SpawnParams);
				//side = 0;
			}
		}
	}
}

void AMLevelGenerator::SpawnOuterWalls(float startPosition, FActorSpawnParameters& SpawnParams)
{
	FVector location1(-startPosition, -startPosition, wallFloor);
	AActor* outer = GetWorld()->SpawnActor<AActor>(outerWall, location1, FRotator::ZeroRotator, SpawnParams);

	float halfSize = distanceBetweenCells * 0.5f;

	for (int i = 0; i < maxSize; i++)
	{
		UInstancedStaticMeshComponent* newWall = outer->FindComponentByClass<UInstancedStaticMeshComponent>();

		FVector location(startPosition - (i * halfSize), startPosition + halfSize, 0.0f);
		newWall->AddInstance(FTransform(location));

		location = FVector((startPosition + halfSize) - (i * halfSize), startPosition - (halfSize * (maxSize - 1)), 0.0f);
		newWall->AddInstance(FTransform(location));
		location = FVector(startPosition - (halfSize * (maxSize - 1)), startPosition - (i * halfSize), 0.0f);
		newWall->AddInstance(FTransform(location));
		location = FVector(startPosition + halfSize, startPosition + halfSize - (i * halfSize), 0.0f);
		newWall->AddInstance(FTransform(location));
	}
}

void AMLevelGenerator::SpawnTreasure(FActorSpawnParameters& SpawnParams, TArray<Cell>& cellGrid, float startPosition)
{
	if (treasureLocation == 0)
	{
		//Hard set for now
		GetWorld()->SpawnActor<AActor>(treasure, FVector(200, 200, 100), FRotator::ZeroRotator, SpawnParams);
	}
	else if (treasureLocation == 1)
	{
		FVector2D p = cellGrid[cellGrid.Num() - 1].position;
		FVector location(startPosition - (p.X * distanceBetweenCells), startPosition - (p.Y * distanceBetweenCells), 100);
		GetWorld()->SpawnActor<AActor>(treasure, location, FRotator::ZeroRotator, SpawnParams);
	}
	else if (treasureLocation == 2)
	{
		int randomIndex = FMath::RandRange(45, cellGrid.Num() - 1);

		FVector2D p = cellGrid[randomIndex].position;
		FVector location(startPosition - (p.X * distanceBetweenCells), startPosition - (p.Y * distanceBetweenCells), 100);
		GetWorld()->SpawnActor<AActor>(treasure, location, FRotator::ZeroRotator, SpawnParams);
	}
}

void AMLevelGenerator::SpawnPlayer(float startPosition, FActorSpawnParameters& SpawnParams)
{
	FVector position(startPosition, startPosition, 100);
	FRotator rotation(0.0f, -90.0f, 0.0f);
	auto player = GetWorld()->SpawnActor<AMPlayerCharacter>(thePlayer, position, rotation, SpawnParams);

	FHitResult OutHit;

	FVector Start = player->GetActorLocation();

	FVector ForwardVector = player->GetActorForwardVector();
	FVector End = ((ForwardVector * 200.0f) + Start);
	FCollisionQueryParams CollisionParams;

	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams))
	{
		if (OutHit.bBlockingHit)
		{
			FRotator NewRotation = player->GetActorRotation();
			NewRotation.Yaw -= 90.0f;
			player->SetActorRotation(NewRotation);
			UE_LOG(LogTemp, Warning, TEXT("Heeeeeys"));
		}
	}
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->Possess(player);
}


