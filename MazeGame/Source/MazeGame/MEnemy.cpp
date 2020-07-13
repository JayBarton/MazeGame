// Fill out your copyright notice in the Description page of Project Settings.


#include "MEnemy.h"
#include "AIController.h"
#include "Runtime/Engine/Classes/Engine/TargetPoint.h"

#include "Blueprint/AIBlueprintHelperLibrary.h" 

#include "DrawDebugHelpers.h"



#include "GameFramework/Actor.h" 

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
	
	FTimerHandle TimerHandle_CheckTurn;
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


	//FVector SideStart = Start + (GetActorForwardVector() * 90.0f);
	FVector SideStart = End;
	FVector RightLine = SideStart + (GetActorRightVector() * 200.0f);
	FVector LeftLine = SideStart + (GetActorRightVector() * -200.0f);

	DrawDebugLine(GetWorld(), SideStart, RightLine, FColor::Red, false, 1, 0, 1);
	DrawDebugLine(GetWorld(), SideStart, LeftLine, FColor::Blue, false, 1, 0, 1);
	turned = false;

	if (checkRight)
	{
		if (!GetWorld()->LineTraceSingleByChannel(OutHit, SideStart, RightLine, ECC_Visibility, CollisionParams))
		{
			checkRight = false;
			tryTurn = true;
		}
	}
	else
	{
		if (GetWorld()->LineTraceSingleByChannel(OutHit, SideStart, RightLine, ECC_Visibility, CollisionParams))
		{
			if (OutHit.bBlockingHit)
			{
				checkRight = true;
				if (tryTurn)
				{
					//turnRight = 1;
					turnRight = FMath::RandRange(0, 1);
				}
			}
		}
	}

	if (checkLeft)
	{
		if (!GetWorld()->LineTraceSingleByChannel(OutHit, SideStart, LeftLine, ECC_Visibility, CollisionParams))
		{
			checkLeft = false;
			tryTurn = true;
		}
	}
	else
	{
		if (GetWorld()->LineTraceSingleByChannel(OutHit, SideStart, LeftLine, ECC_Visibility, CollisionParams))
		{
			if (OutHit.bBlockingHit)
			{
				checkLeft = true;
				if (tryTurn)
				{

					//turnLeft = 1;
					turnLeft = FMath::RandRange(0, 1);
				}
			}
		}
	}

	if (turnRight > 0 && turnLeft > 0)
	{
		int pick = FMath::RandRange(0, 1);
		if (pick > 0)
		{
			turnLeft = 0;
			UE_LOG(LogTemp, Warning, TEXT("111"));
		}
		else
		{
			turnRight = 0;
			UE_LOG(LogTemp, Warning, TEXT("222"));
		}
	}
	if (turnRight > 0)
	{
		TurnDirection(90.0f);
		turned = true;
		tryTurn = false;
		checkRight = false;
		checkLeft = false;
		turnRight = 0;

		UE_LOG(LogTemp, Warning, TEXT("AAA"));

	}
	else if (turnLeft > 0)
	{
		TurnDirection(-90.0f);
		turned = true;
		tryTurn = false;
		checkLeft = false;
		checkRight = false;
		turnLeft = 0;

		UE_LOG(LogTemp, Warning, TEXT("BBB"));
	}

	if(!turned)
	{
		if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams))
		{
			if (OutHit.bBlockingHit)
			{
				checkRight = true;
				//Enemy hit a wall and needs to turn
				int dir = 0;
				float turnAngle = 90.0f;
				bool rightClear = true;
				bool leftClear = true;

				SideStart = Start + (GetActorForwardVector() * 50.0f);

				 RightLine = SideStart + (GetActorRightVector() * 200.0f);
				 LeftLine = SideStart + (GetActorRightVector() * -200.0f);

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
				else if (!rightClear && !leftClear)
				{
					dir = 1;
					turnAngle *= 2;
					UE_LOG(LogTemp, Warning, TEXT("back"));
				}

				TurnDirection(turnAngle * dir);

				turned = true;
				tryTurn = false;
				checkRight = false;
				checkLeft = false;

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
}

void AMEnemy::TurnDirection(float angle)
{
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += angle;
	SetActorRotation(NewRotation);
}

