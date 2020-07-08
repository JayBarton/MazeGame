// Fill out your copyright notice in the Description page of Project Settings.


#include "MEnemy.h"
#include "AIController.h"
#include "Runtime/Engine/Classes/Engine/TargetPoint.h"

#include "Blueprint/AIBlueprintHelperLibrary.h" 

// Sets default values
AMEnemy::AMEnemy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), FVector(-700.0f, 700.0f, 0.0f));
}

// Called every frame
void AMEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


