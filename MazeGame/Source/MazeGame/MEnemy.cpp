// Fill out your copyright notice in the Description page of Project Settings.


#include "MEnemy.h"
#include "AIController.h"

#include "Perception/PawnSensingComponent.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NavigationSystem.h"

#include "DrawDebugHelpers.h"

#include "Kismet/GameplayStatics.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "MPlayerCharacter.h"

#include "MazeGameGameModeBase.h"


// Sets default values
AMEnemy::AMEnemy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	//Guard will not hear if they have already seen
	PawnSensingComp->OnSeePawn.AddDynamic(this, &AMEnemy::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AMEnemy::OnNoiseHeard);

	halfAttackLength = attackLength * 0.5f;
	attackTime = 0.0f;
}

// Called when the game starts or when spawned
void AMEnemy::BeginPlay()
{
	Super::BeginPlay();
	thePlayer = Cast<AMPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

// Called every frame
void AMEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector2D playerPosition(thePlayer->GetActorLocation().X, thePlayer->GetActorLocation().Y);
	if (foundPlayer)
	{
		FollowPlayer(playerPosition);
	}
	else if (attacking)
	{
		attackTime += DeltaTime;
		if (attackTime > halfAttackLength)
		{
			FHitResult OutHit;


			FVector ForwardVector = GetActorForwardVector();
			FVector Start = GetActorLocation() + 80 * ForwardVector;

			FVector End = ((ForwardVector * attackRange) + Start);
			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(this);

			DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 1);
			auto shape = FCollisionShape::MakeBox(FVector(100, 100, 100));
			DrawDebugBox(GetWorld(), Start, FVector(100, 100, 100), FColor::Red, false, 1.0f, 0.0f, 1.0f);
			DrawDebugBox(GetWorld(), End, FVector(100, 100, 100), FColor::Red, false, 1.0f, 0.0f, 1.0f);
			if(GetWorld()->SweepSingleByChannel(OutHit, Start, End, FQuat::Identity, ECC_GameTraceChannel1, shape, CollisionParams))
			//if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_GameTraceChannel1, CollisionParams))
			{
				AMazeGameGameModeBase* GM = Cast<AMazeGameGameModeBase>(GetWorld()->GetAuthGameMode());
				if (GM)
				{
					GM->CompleteLevel(false, this);
				}
				//UE_LOG(LogTemp, Warning, TEXT("HIT"))
			}
		}
		if (attackTime > attackLength)
		{
			attacking = false;
			FoundPlayer();
			attackTime = 0.0f;
		}
	}
	else
	{
		WanderAroundPlayer(playerPosition);
	}

	seenTimer += DeltaTime;
	if (seenTimer >= 2.0f)
	{
		canSee = false;
	}
	heardTimer += DeltaTime;
}

//Set the enemy back to the wander state if they are too far from the player
void AMEnemy::FollowPlayer(FVector2D& playerPosition)
{
	FVector2D currentPosition(GetActorLocation().X, GetActorLocation().Y);
	float distance = (currentPosition - playerPosition).Size();

	if (distance > 1000.0f)
	{
		if (!canSee)
		{
			foundPlayer = false;
			if (AController* AI = GetController())
			{
				AI->StopMovement();
			}
			Reset(currentPosition);

			GetCharacterMovement()->MaxWalkSpeed = 300;
			thePlayer->bChased = false;

			UE_LOG(LogTemp, Warning, TEXT("Lost you..."));
		}
	}
	else
	{
			if (distance < attackDistance)
			{
				foundPlayer = false;
				attacking = true;
				if (AController* AI = GetController())
				{
					UE_LOG(LogTemp, Warning, TEXT("Attack"));

					AI->StopMovement();
				}
			}
	}
}

//Wander behavior
void AMEnemy::WanderAroundPlayer(FVector2D& playerPosition)
{
	FVector2D currentPosition(GetActorLocation().X, GetActorLocation().Y);
	if (moveToPosition)
	{
		MoveBackToPlayer(currentPosition, playerPosition);
	}
	else
	{
		Wander(currentPosition, playerPosition);
	}
}

//Reset back to wandering once close enough to the player
void AMEnemy::MoveBackToPlayer(FVector2D& currentPosition, FVector2D& playerPosition)
{
	if ((currentPosition - playerPosition).Size() < 1000.0f)
	{
		if (AController* AI = GetController())
		{
			AI->StopMovement();
		}

		UE_LOG(LogTemp, Warning, TEXT("Wandering"));

		Reset(currentPosition);

		moveToPosition = false;
	}
}

//Wander around the player's position
void AMEnemy::Wander(FVector2D& currentPosition, FVector2D& playerPosition)
{
	//If the enemy is too far from the player, move back towards them
	if ((currentPosition - playerPosition).Size() > 2000.0f)
	{
		moveToPosition = true;
		UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), thePlayer);
		UE_LOG(LogTemp, Warning, TEXT("Seeking"));

	}
	//Else wander around the maze
	else
	{
		GetForwardDirection();
		AddMovementInput(GetActorForwardVector(), 1.0f);
	}
}

//Reset enemy position and rotation for wandering
void AMEnemy::Reset(FVector2D& currentPosition)
{
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw = FMath::RoundToFloat(NewRotation.Yaw / 90.0f) * 90.0f;
	SetActorRotation(NewRotation);

	UE_LOG(LogTemp, Warning, TEXT("A: %f"), NewRotation.Yaw);
	UE_LOG(LogTemp, Warning, TEXT("B: %f"), GetActorRotation().Yaw);
	UE_LOG(LogTemp, Warning, TEXT("x: %f, y: %f"), GetActorForwardVector().X, GetActorForwardVector().Y);

	currentPosition.X = FMath::RoundToFloat(currentPosition.X / 100.0f) * 100.0f;
	currentPosition.Y = FMath::RoundToFloat(currentPosition.Y / 100.0f) * 100.0f;
	SetActorLocation(FVector(currentPosition.X, currentPosition.Y, GetActorLocation().Z));
}

void AMEnemy::GetForwardDirection()
{
	bool turned = false;

	CheckSides(turned);

	if(!turned)
	{
		CheckFront();
	}
}

void AMEnemy::CheckSides(bool& turned)
{
	FHitResult OutHit;

	FVector Start = GetActorLocation();

	FVector ForwardVector = GetActorForwardVector();
	FVector End = ((ForwardVector * 100.0f) + Start);
	FCollisionQueryParams CollisionParams;

	FVector SideStart = GetActorLocation() + (GetActorForwardVector() * 90.0f);
	//FVector SideStart = End;
	FVector RightLine = SideStart + (GetActorRightVector() * 200.0f);
	FVector LeftLine = SideStart + (GetActorRightVector() * -200.0f);

	DrawDebugLine(GetWorld(), SideStart, RightLine, FColor::Red, false, 1, 0, 1);
	DrawDebugLine(GetWorld(), SideStart, LeftLine, FColor::Blue, false, 1, 0, 1);

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
		}
		else
		{
			turnRight = 0;

		}
		//UE_LOG(LogTemp, Warning, TEXT("rand early"));
	}
	if (turnRight > 0)
	{
		TurnDirection(90.0f);
		turned = true;
		turnRight = 0;

		//UE_LOG(LogTemp, Warning, TEXT("right early"));

	}
	else if (turnLeft > 0)
	{
		TurnDirection(-90.0f);
		turned = true;
		turnLeft = 0;

		//	UE_LOG(LogTemp, Warning, TEXT("left early"));
	}
}

void AMEnemy::CheckFront()
{
	FHitResult OutHit;

	FVector Start = GetActorLocation();

	FVector ForwardVector = GetActorForwardVector();
	FVector End = ((ForwardVector * 100.0f) + Start);
	FCollisionQueryParams CollisionParams;

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

			FVector SideStart = Start + (GetActorForwardVector() * 50.0f);

			FVector RightLine = SideStart + (GetActorRightVector() * 200.0f);
			FVector LeftLine = SideStart + (GetActorRightVector() * -200.0f);

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
				//UE_LOG(LogTemp, Warning, TEXT("rand"));
			}
			//If only one is clear, turn in that direction
			else if (rightClear && !leftClear)
			{
				dir = 1;
				//UE_LOG(LogTemp, Warning, TEXT("right"));
			}
			else if (!rightClear && leftClear)
			{
				dir = -1;
				//	UE_LOG(LogTemp, Warning, TEXT("left"));
			}
			//If neither are clear, turn around
			else if (!rightClear && !leftClear)
			{
				dir = 1;
				turnAngle *= 2;
				//	UE_LOG(LogTemp, Warning, TEXT("back"));
			}

			TurnDirection(turnAngle * dir);
		}
	}
}

void AMEnemy::TurnDirection(float angle)
{
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw = FMath::RoundToFloat(NewRotation.Yaw + angle);
	SetActorRotation(NewRotation);
	//UE_LOG(LogTemp, Warning, TEXT("x: %f, y: %f"), NewRotation.Vector().ForwardVector.X, NewRotation.Vector().ForwardVector.Y);

	tryTurn = false;
	checkRight = false;
	checkLeft = false;

}

void AMEnemy::OnPawnSeen(APawn* SeenPawn)
{
	if (SeenPawn)
	{
		if (seenTimer >= 2.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("I see you"));
			DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12, FColor::Red, false, 10.0f);
			heardTimer = 0.0f;

		}
		canSee = true;
		seenTimer = 0.0f;

		FoundPlayer();
	}
}

void AMEnemy::OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volumne)
{
	/*
	if heard == false
	heard = true
	seen = false
	output message
	*/
	if (heardTimer >= 2.0f)
	{
		DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Green, false, 10.0f);
		UE_LOG(LogTemp, Warning, TEXT("Yes I hear you"));
		seenTimer = 0.0f;

	}
	heardTimer = 0.0f;
	canSee = false;

	FoundPlayer();

}

void AMEnemy::FoundPlayer()
{
	if (!foundPlayer && !attacking)
	{
		foundPlayer = true;
		if (AController* AI = GetController())
		{
			UE_LOG(LogTemp, Warning, TEXT("4"));

			AI->StopMovement();
		}
		GetCharacterMovement()->MaxWalkSpeed = 600;
		UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), thePlayer);
		thePlayer->bChased = true;
	}
}

