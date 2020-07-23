// Fill out your copyright notice in the Description page of Project Settings.


#include "MTreasure.h"
#include "Components/SphereComponent.h" 
#include "Kismet/GameplayStatics.h"
#include "MPlayerCharacter.h"

// Sets default values
AMTreasure::AMTreasure()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = MeshComp;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetupAttachment(MeshComp);

}

// Called when the game starts or when spawned
void AMTreasure::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMTreasure::PlayEffects()
{
	UGameplayStatics::SpawnEmitterAtLocation(this, PickupFX, GetActorLocation());
}

void AMTreasure::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);


	AMPlayerCharacter* player = Cast<AMPlayerCharacter>(OtherActor);
	if (player)
	{
		player->bHasTreasure = true;
		PlayEffects();

		Destroy();
	}

}


