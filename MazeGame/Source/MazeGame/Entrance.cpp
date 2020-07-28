// Fill out your copyright notice in the Description page of Project Settings.


#include "Entrance.h"
#include "MPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "MazeGameGameModeBase.h"

// Sets default values
AEntrance::AEntrance()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	OverlapComp = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapComp"));
	OverlapComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapComp->SetBoxExtent(FVector(200.0f));
	RootComponent = OverlapComp;

	OverlapComp->SetHiddenInGame(false);

	OverlapComp->OnComponentBeginOverlap.AddDynamic(this, &AEntrance::HandleOverlap);

	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->DecalSize = FVector(200.0f);
	DecalComp->SetupAttachment(RootComponent);

}

void AEntrance::HandleOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMPlayerCharacter* player = Cast<AMPlayerCharacter>(OtherActor);

	if (player)
	{
		if (player->bHasTreasure)
		{
			UE_LOG(LogTemp, Warning, TEXT("Got it"));
			AMazeGameGameModeBase* GM = Cast<AMazeGameGameModeBase>(GetWorld()->GetAuthGameMode());
			if (GM)
			{
				GM->CompleteLevel();
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("nope"));

		}
	}
}