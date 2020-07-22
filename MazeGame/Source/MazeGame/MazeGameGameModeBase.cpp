// Fill out your copyright notice in the Description page of Project Settings.


#include "MazeGameGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "MEnemy.h"

void AMazeGameGameModeBase::CompleteLevel()
{
	UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->SetActorTickEnabled(false);
    UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->DisableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMEnemy::StaticClass(), FoundActors);

    for (auto& TActor : FoundActors)
    {
        AMEnemy* MyActor = Cast<AMEnemy>(TActor);
        MyActor->GetController()->UnPossess();
    }

    OnLevelCompleted();
}
