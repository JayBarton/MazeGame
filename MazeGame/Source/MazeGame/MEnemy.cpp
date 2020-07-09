// Fill out your copyright notice in the Description page of Project Settings.


#include "MEnemy.h"
#include "AIController.h"
#include "Runtime/Engine/Classes/Engine/TargetPoint.h"

#include "Blueprint/AIBlueprintHelperLibrary.h" 

#include "DrawDebugHelpers.h"

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
	
	//UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), FVector(-700.0f, 700.0f, 0.0f));
}

// Called every frame
void AMEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GetForwardDirection();

	AddMovementInput(GetActorForwardVector(), 1.0f);
}

void AMEnemy::GetForwardDirection()
{
	FHitResult OutHit;

	FVector Start = GetActorLocation();

	FVector ForwardVector = GetActorForwardVector();
	FVector End = ((ForwardVector * 100.0f) + Start);
	FCollisionQueryParams CollisionParams;

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 1);

	FVector RightLine = Start + (GetActorRightVector() * 200.0f);
	FVector LeftLine = Start + (GetActorRightVector() * -200.0f);

	DrawDebugLine(GetWorld(), Start, RightLine, FColor::Red, false, 1, 0, 1);
	DrawDebugLine(GetWorld(), Start, LeftLine, FColor::Blue, false, 1, 0, 1);

	//Check if there are paths to the side
	//This method doesn't work, as the enemy checks it's side every tick, and eventually makes the turn, causing a turn every time it approaches a gap
	/*if (!GetWorld()->LineTraceSingleByChannel(OutHit, Start, RightLine, ECC_Visibility, CollisionParams))
	{
		UE_LOG(LogTemp, Warning, TEXT("AAA"));
		int turn = FMath::RandRange(0, 1);
		if (turn)
		{
			UE_LOG(LogTemp, Warning, TEXT("BBB"));

			FRotator NewRotation = GetActorRotation();
			NewRotation.Yaw += 90.0f;
			SetActorRotation(NewRotation);
		}
	}*/

	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams))
	{
		if (OutHit.bBlockingHit)
		{
			//Enemy hit a wall and needs to turn
			int dir = 0;
			float turnAngle = 90.0f;
			bool rightClear = true;
			bool leftClear = true;

			//Check if the right and/or left of the enemy is clear
			if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, RightLine, ECC_Visibility, CollisionParams))
			{
				if (OutHit.bBlockingHit)
				{
					rightClear = false;
				}
			}
			if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, LeftLine, ECC_Visibility, CollisionParams))
			{
				if (OutHit.bBlockingHit)
				{
					leftClear = false;
				}
			}

			//If both are clear, pick a random direction to turn
			if (rightClear && leftClear)
			{
				dir = FMath::RandRange(0, 1);
				if (dir == 0)
				{
					dir = -1;
				}
				UE_LOG(LogTemp, Warning, TEXT("rand"));
			}
			//If only one is clear, turn in that direction
			else if (rightClear && !leftClear)
			{
				dir = 1;
				UE_LOG(LogTemp, Warning, TEXT("right"));
			}
			else if (!rightClear && leftClear)
			{
				dir = -1;
				UE_LOG(LogTemp, Warning, TEXT("left"));
			}
			//If neither are clear, turn around
			else if(!rightClear && !leftClear)
			{
				dir = 1;
				turnAngle *= 2;
				UE_LOG(LogTemp, Warning, TEXT("back"));
			}

			FRotator NewRotation = GetActorRotation();
			NewRotation.Yaw += turnAngle * dir;
			SetActorRotation(NewRotation);

			//Make sure enemy isn't turning into a wall
		//	ForwardVector = GetActorForwardVector();
		//	End = ((ForwardVector * 150.0f) + Start);
			/*if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams))
			{
				if (OutHit.bBlockingHit)
				{
					NewRotation.Yaw += 90.0f * dir;
					SetActorRotation(NewRotation);
				}
			}*/
		}
	}
}


