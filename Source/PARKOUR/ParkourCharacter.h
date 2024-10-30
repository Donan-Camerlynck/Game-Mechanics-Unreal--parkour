// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ParkourCharacter.generated.h"

UCLASS()
class PARKOUR_API AParkourCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement) class UPlayerCharacterMovementComponent* PlayerCharacterMovementComponent;


public:
	// Sets default values for this character's properties
	AParkourCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void CheckJumpInput(float DeltaTime) override;
	virtual bool CanJumpInternal_Implementation() const override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintPure) UPlayerCharacterMovementComponent* GetPlayerCharacterMovement() const;
	
	FCollisionQueryParams GetIgnoreCharacterParams() const;

};
