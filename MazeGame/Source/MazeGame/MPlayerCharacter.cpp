// Fill out your copyright notice in the Description page of Project Settings.


#include "MPlayerCharacter.h"

#include "GameFramework/PlayerController.h" 
#include "Components/PawnNoiseEmitterComponent.h"

#include "Kismet/GameplayStatics.h" 


// Sets default values
AMPlayerCharacter::AMPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	NoiseEmitterComponent = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("NoiseEmitterComponent"));

	bHasTreasure = false;
}

// Called when the game starts or when spawned
void AMPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMPlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AMPlayerCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AMPlayerCharacter::AddControllerPitchInput);

}

void AMPlayerCharacter::MoveForward(float Value)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, Value);

	UGameplayStatics::GetPlayerController(GetWorld(), 0)->ClientPlayCameraShake(CameraBob, Value);
	
}

void AMPlayerCharacter::MoveRight(float Value)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
/*	float forward = InputComponent->GetAxisValue(TEXT("MoveForward"));
	
	if (forward != 0)
	{
		float direction = 0;
		if (Value > 0)
		{
			direction = 1;
		}
		else if(Value < 0)
		{
			direction = -1;
		}
		AddMovementInput(Direction, forward * direction);
	}
	else
	{
		AddMovementInput(Direction, Value);
	}*/
	AddMovementInput(Direction, Value);
}

