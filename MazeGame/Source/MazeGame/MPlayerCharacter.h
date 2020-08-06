// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MPlayerCharacter.generated.h"

class UCameraShake;
class UPawnNoiseEmitterComponent;
class UCameraComponent;


UCLASS()
class MAZEGAME_API AMPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMPlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void MoveForward(float Value);

	// Handles input for moving right and left.
	UFUNCTION()
	void MoveRight(float Value);

	//Sets up player being chased

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UPawnNoiseEmitterComponent* NoiseEmitterComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	TSubclassOf<UCameraShake> CameraBob;

	UPROPERTY(BlueprintReadonly, Category = "Player")
	bool bHasTreasure;
	UPROPERTY(BlueprintReadonly, Category = "Player")
	bool bChased;

	UPROPERTY(BlueprintReadonly, Category = "Player")
	float walkSpeed = 500.0f;
	UPROPERTY(BlueprintReadonly, Category = "Player")
	float maxRunSpeed = 700.0f;
	UPROPERTY(BlueprintReadonly, Category = "Player")
	float currentRunSpeed = 700.0f;

	UPROPERTY(BlueprintReadonly, Category = "Player")
	float stamina = 100.0f;
	//Rate that running drains stamina
	UPROPERTY(EditAnywhere, Category = "Player")
	float runDrain = 5.0f;
	//rate that stamina recovers
	UPROPERTY(EditAnywhere, Category = "Player")
	float recoverGain = 2.5f;
};
