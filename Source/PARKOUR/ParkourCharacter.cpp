// Fill out your copyright notice in the Description page of Project Settings.


#include "ParkourCharacter.h"
#include "PlayerCharacterMovementComponent.h"

// Sets default values
AParkourCharacter::AParkourCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UPlayerCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PlayerCharacterMovementComponent = Cast<UPlayerCharacterMovementComponent>(GetCharacterMovement());

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AParkourCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AParkourCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AParkourCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UPlayerCharacterMovementComponent* AParkourCharacter::GetPlayerCharacterMovement() const
{
	return PlayerCharacterMovementComponent;
}

FCollisionQueryParams AParkourCharacter::GetIgnoreCharacterParams() const
{
	FCollisionQueryParams params;

	TArray<AActor*> characterChildren;
	GetAllChildActors(characterChildren);
	params.AddIgnoredActors(characterChildren);
	params.AddIgnoredActor(this);

	return params;
}

void AParkourCharacter::CheckJumpInput(float DeltaTime)
{
	JumpCurrentCountPreJump = JumpCurrentCount;

	if (PlayerCharacterMovementComponent)
	{
		if (bPressedJump)
		{
			// If this is the first jump and we're already falling,
			// then increment the JumpCount to compensate.
			const bool bFirstJump = JumpCurrentCount == 0;
			if (bFirstJump && PlayerCharacterMovementComponent->IsFalling())
			{
				JumpCurrentCount++;
			}

			const bool bDidJump = CanJump() && PlayerCharacterMovementComponent->DoJump(bClientUpdating);
			if (bDidJump)
			{
				// Transition from not (actively) jumping to jumping.
				if (!bWasJumping)
				{
					JumpCurrentCount++;
					JumpForceTimeRemaining = GetJumpMaxHoldTime();
					OnJumped();
				}
			}

			bWasJumping = bDidJump;
		}
	}
}

bool AParkourCharacter::CanJumpInternal_Implementation() const
{
	return JumpIsAllowedInternal();
}



