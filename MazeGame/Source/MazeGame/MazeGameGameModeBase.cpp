// Fill out your copyright notice in the Description page of Project Settings.


#include "MazeGameGameModeBase.h"
#include "MEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h" 



#include "UObject/UObjectGlobals.h" 
#include "Engine/StaticMesh.h" 
void AMazeGameGameModeBase::CompleteLevel(bool success, APawn* InstigatorPawn)
{
    auto playerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    playerPawn->SetActorTickEnabled(false);
    playerPawn->DisableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMEnemy::StaticClass(), FoundActors);

    for (auto& TActor : FoundActors)
    {
        AMEnemy* MyActor = Cast<AMEnemy>(TActor);
        MyActor->SetActorTickEnabled(false);
        MyActor->GetController()->UnPossess();
    }

    if (!success)
    {
        auto PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PC)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
            FVector location = InstigatorPawn->GetActorLocation() + InstigatorPawn->GetActorForwardVector() * 300.0f;
            location.Z += 75.0f;
            FRotator test = UKismetMathLibrary::FindLookAtRotation(location, InstigatorPawn->GetActorLocation());
            AActor* viewActor = GetWorld()->SpawnActor<AActor>(view, location, test, SpawnParams);


            PC->SetViewTargetWithBlend(viewActor, 0.5f, EViewTargetBlendFunction::VTBlend_Cubic);
        }
    }

    OnLevelCompleted(success);
}
