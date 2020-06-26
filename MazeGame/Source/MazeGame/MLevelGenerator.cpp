// Fill out your copyright notice in the Description page of Project Settings.


#include "MLevelGenerator.h"

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
	grid.Init(FVector(0), 100);
	for (int i = 0; i < 11; i++)
	{
		for (int c = 0; c < 11; c++)
		{
			if (i % 2 == 0 || c % 2 == 0)
			{
				//wall
				FVector location(450.0f - (i * 100), 450.0f - (c * 100), 50.0f);

				AActor* newWall = GetWorld()->SpawnActor<AActor>(wall, location, FRotator::ZeroRotator, SpawnParams);
			}
		}
	}
}

// Called every frame
void AMLevelGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

