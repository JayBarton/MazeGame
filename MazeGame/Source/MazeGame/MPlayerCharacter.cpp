// Fill out your copyright notice in the Description page of Project Settings.


#include "MPlayerCharacter.h"

#include "GameFramework/PlayerController.h" 
#include "Components/PawnNoiseEmitterComponent.h"

#include "Kismet/GameplayStatics.h" 
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"


// Sets default values
AMPlayerCharacter::AMPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	NoiseEmitterComponent = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("NoiseEmitterComponent"));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	CameraComponent->SetupAttachment(GetCapsuleComponent());
	CameraComponent->RelativeLocation = FVector(0, 0, BaseEyeHeight); // Position the camera
	CameraComponent->bUsePawnControlRotation = true;

	bHasTreasure = false;
	bChased = false;

	currentRunSpeed = maxRunSpeed;
	stamina = 100;
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

	if (bChased)
	{
		stamina -= runDrain * DeltaTime;

		stamina = FMath::Clamp(stamina, 0.0f, 100.0f);
		currentRunSpeed = FMath::Lerp(walkSpeed, maxRunSpeed, stamina / 100);
	}
	else
	{
		stamina += recoverGain * DeltaTime;

		stamina = FMath::Clamp(stamina, 0.0f, 100.0f);
	}
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
	if (Value != 0)
	{
		AddMovementInput(GetActorForwardVector(), Value);
		if (bChased)
		{
			if (Value > 0)
			{
				GetCharacterMovement()->MaxWalkSpeed = currentRunSpeed;
			}
			else
			{
				GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
			}
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
		}

		UGameplayStatics::GetPlayerController(GetWorld(), 0)->ClientPlayCameraShake(CameraBob, Value);
	}
	
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
	if (Value != 0)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

