#pragma once

#include "CoreMinimal.h"
#include "ParkourCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerCharacterMovementComponent.generated.h"


UENUM(BlueprintType)

enum ECustomMovementMode
{
	CMOVE_None			UMETA(Hidden),
	CMOVE_Slide			UMETA(DisplayName = "Slide"),
	CMOVE_MAX			UMETA(Hidden),
};

UCLASS()

class PARKOUR_API UPlayerCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

		class FSavedMove_Parkour : public FSavedMove_Character
	{
		typedef FSavedMove_Character Super;

		uint8 Saved_bWantsToSprint : 1;

		uint8 Saved_bPrevWantsToCrouch : 1;

		uint8 Saved_bWantsToSlide : 1;

		virtual bool CanCombineWith(const FSavedMovePtr& newMove, ACharacter* inCharacter, float maxDelta) const override;
		virtual void Clear() override;
		virtual uint8 GetCompressedFlags() const override;
		virtual void SetMoveFor(ACharacter* C, float inDeltaTime, FVector const& newAccel, FNetworkPredictionData_Client_Character& clientData) override;
		virtual void PrepMoveFor(ACharacter* C) override;

	};

	class FNetworkPredictionData_Client_Parkour : public FNetworkPredictionData_Client_Character
	{
	public:
		FNetworkPredictionData_Client_Parkour(const UCharacterMovementComponent& clientMovement);

		typedef FNetworkPredictionData_Client_Character Super;

		virtual FSavedMovePtr AllocateNewMove() override;
	};

	UPROPERTY(EditDefaultsOnly) float sprint_MaxWalkSpeed;
	UPROPERTY(EditDefaultsOnly) float walk_MaxWalkSpeed;

	UPROPERTY(Transient) AParkourCharacter* parkourCharacterOwner;

	UPROPERTY(EditDefaultsOnly) float minSlideSpeed = 1000.f;
	UPROPERTY(EditDefaultsOnly) float maxSlideSpeed = 3000.f;
	UPROPERTY(EditDefaultsOnly) float slideEnterImpulse = 750.f;
	UPROPERTY(EditDefaultsOnly) float slideGravityForce = 4000.f;
	UPROPERTY(EditDefaultsOnly) float slideFrictionFactor = .26f;
	UPROPERTY(EditDefaultsOnly) float brakingDecelerationSliding = 5000.f;

	bool Safe_bWantsToSprint;
	bool Safe_bPrevWantsToCrouch;
	bool Safe_bWantsToSlide;

public:
	UPlayerCharacterMovementComponent();
		

	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	virtual bool CanAttemptJump() const override;
	

	UFUNCTION(BlueprintCallable) void SprintPressed();
	UFUNCTION(BlueprintCallable) void SprintReleased();

	UFUNCTION(BlueprintCallable) void CrouchPressed();

	UFUNCTION(BlueprintCallable) void SlidePressed();


	UFUNCTION(BlueprintPure) bool IsCustomMovementMode(ECustomMovementMode inCustomMovementMode) const;
	virtual bool DoJump(bool bReplayingMoves) override;
	virtual bool IsMovingOnGround() const override;
protected:
	virtual void UpdateFromCompressedFlags(uint8 flags) override;
	virtual void InitializeComponent() override;
	virtual void OnMovementUpdated(float deltaSeconds, const FVector& oldLocation, const FVector& oldVelocity) override;

	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual void Launch(FVector const& LaunchVel) override;
	
	virtual bool CanCrouchInCurrentState() const override;


	


private:
	void EnterSlide();
	void ExitSlide();
	void PhysSlide(float deltaTime, int32 iterations);
	bool GetSlideSurface(FHitResult& hit) const;
};

