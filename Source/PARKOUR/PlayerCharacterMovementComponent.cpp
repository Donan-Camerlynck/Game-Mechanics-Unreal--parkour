#include "PlayerCharacterMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "VT/VirtualTextureBuildSettings.h"

bool UPlayerCharacterMovementComponent::FSavedMove_Parkour::CanCombineWith(const FSavedMovePtr& newMove,
                                                                           ACharacter* inCharacter, float maxDelta) const
{
	FSavedMove_Parkour* pNewParkourMove = static_cast<FSavedMove_Parkour*>(newMove.Get());
	if (Saved_bWantsToSprint != pNewParkourMove->Saved_bWantsToSprint) //if not the same, move is not the same
	{
		return false;
	}

	if(Saved_bWantsToSlide != pNewParkourMove->Saved_bWantsToSlide)
	{
		return false;
	}

	return FSavedMove_Character::CanCombineWith(newMove, inCharacter, maxDelta);
}

void UPlayerCharacterMovementComponent::FSavedMove_Parkour::Clear()
{
	FSavedMove_Character::Clear();

	Saved_bWantsToSprint = false;
	Saved_bWantsToSlide = false;
	Saved_bPrevWantsToCrouch = false;
}

uint8 UPlayerCharacterMovementComponent::FSavedMove_Parkour::GetCompressedFlags() const
{
	uint8 result = Super::GetCompressedFlags();

	if (Saved_bWantsToSprint)
	{
		result |= FLAG_Custom_0;
	}

	if (Saved_bWantsToSlide)
	{
		result |= FLAG_Custom_1;
	}

	return result;
}

void UPlayerCharacterMovementComponent::FSavedMove_Parkour::SetMoveFor(ACharacter* C, float inDeltaTime,
	FVector const& newAccel, FNetworkPredictionData_Client_Character& clientData)
{
	FSavedMove_Character::SetMoveFor(C, inDeltaTime, newAccel, clientData);

	UPlayerCharacterMovementComponent* CharacterMovement = Cast<UPlayerCharacterMovementComponent>(C->GetCharacterMovement());

	Saved_bWantsToSprint = CharacterMovement->Safe_bWantsToSprint;
	Saved_bPrevWantsToCrouch = CharacterMovement->Safe_bPrevWantsToCrouch;
	Saved_bWantsToSlide = CharacterMovement->Safe_bWantsToSlide;

}

void UPlayerCharacterMovementComponent::FSavedMove_Parkour::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	UPlayerCharacterMovementComponent* CharacterMovement = Cast<UPlayerCharacterMovementComponent>(C->GetCharacterMovement());

	CharacterMovement->Safe_bWantsToSprint = Saved_bWantsToSprint;
	CharacterMovement->Safe_bPrevWantsToCrouch = Saved_bPrevWantsToCrouch;
	CharacterMovement->Safe_bWantsToSlide = Saved_bWantsToSlide;
}

UPlayerCharacterMovementComponent::FNetworkPredictionData_Client_Parkour::FNetworkPredictionData_Client_Parkour(
	const UCharacterMovementComponent& clientMovement) : Super(clientMovement)
{
}

FSavedMovePtr UPlayerCharacterMovementComponent::FNetworkPredictionData_Client_Parkour::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_Parkour()); //not inheriting from UObject so heap allocation possible
}

FNetworkPredictionData_Client* UPlayerCharacterMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != nullptr)

		if (ClientPredictionData == nullptr)
		{
			UPlayerCharacterMovementComponent* MutableThis = const_cast<UPlayerCharacterMovementComponent*>(this);

			MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Parkour(*this);
			MutableThis->ClientPredictionData->MaxClientSmoothingDeltaTime = 92.f;
			MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
		}
	return ClientPredictionData;
}

void UPlayerCharacterMovementComponent::OnMovementUpdated(float deltaSeconds, const FVector& oldLocation,
	const FVector& oldVelocity)
{
	Super::OnMovementUpdated(deltaSeconds, oldLocation, oldVelocity);

	if (MovementMode == MOVE_Walking)
	{
		if (Safe_bWantsToSprint)
		{
			MaxWalkSpeed = sprint_MaxWalkSpeed;
		}
		else
		{
			MaxWalkSpeed = walk_MaxWalkSpeed;
		}
	}	
	Safe_bPrevWantsToCrouch = bWantsToCrouch;
}

void UPlayerCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{	
	if(MovementMode == MOVE_Walking && Safe_bWantsToSlide )
	{
		FHitResult potentialSlideSurface;
		if(Velocity.SizeSquared() > pow(minSlideSpeed, 2) && GetSlideSurface(potentialSlideSurface))
		{
			EnterSlide();			
		}
	}

	if(IsCustomMovementMode(CMOVE_Slide) && !Safe_bWantsToSlide )
	{
		ExitSlide();
	}
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

void UPlayerCharacterMovementComponent::PhysCustom(float deltaTime, int32 iterations)
{
	Super::PhysCustom(deltaTime, iterations);

	switch (CustomMovementMode)
	{
	case CMOVE_Slide:
		PhysSlide(deltaTime, iterations);
		break;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"));
	}
}

void UPlayerCharacterMovementComponent::Launch(FVector const& LaunchVel)
{
	Super::Launch(LaunchVel);
	Safe_bWantsToSlide = false;
}

bool UPlayerCharacterMovementComponent::IsMovingOnGround() const
{
	return Super::IsMovingOnGround() || IsCustomMovementMode(CMOVE_Slide);
}

bool UPlayerCharacterMovementComponent::CanCrouchInCurrentState() const
{
	return Super::CanCrouchInCurrentState() && IsMovingOnGround();
}

void UPlayerCharacterMovementComponent::SprintPressed()
{
	Safe_bWantsToSprint = true;
}

void UPlayerCharacterMovementComponent::SprintReleased()
{
	Safe_bWantsToSprint = false;
}

void UPlayerCharacterMovementComponent::UpdateFromCompressedFlags(uint8 flags)
{
	Super::UpdateFromCompressedFlags(flags);

	Safe_bWantsToSprint = (flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}

void UPlayerCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	parkourCharacterOwner = Cast<AParkourCharacter>(GetOwner());
}

void UPlayerCharacterMovementComponent::EnterSlide()
{
	bWantsToCrouch = true;
	Velocity += Velocity.GetSafeNormal2D() * slideEnterImpulse;
	SetMovementMode(MOVE_Custom, CMOVE_Slide);
}

void UPlayerCharacterMovementComponent::ExitSlide()
{
	bWantsToCrouch = false;
	Safe_bWantsToSlide = false;

	FQuat newRotation = FRotationMatrix::MakeFromXZ(UpdatedComponent->GetForwardVector().GetSafeNormal2D(), FVector::UpVector).ToQuat();
	FHitResult hit;
	SafeMoveUpdatedComponent(FVector::ZeroVector, newRotation, true, hit);
	SetMovementMode(MOVE_Walking);
}

void UPlayerCharacterMovementComponent::PhysSlide(float deltaTime, int32 iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	FHitResult surfaceHit;
	if (!GetSlideSurface(surfaceHit) || Velocity.SizeSquared() < pow(minSlideSpeed, 2))
	{
		ExitSlide();
		StartNewPhysics(deltaTime, iterations);
		return;
	}

	Velocity += slideGravityForce * FVector::DownVector * deltaTime;

	if(FMath::Abs(FVector::DotProduct(Acceleration.GetSafeNormal(), UpdatedComponent->GetRightVector())) > 0.5)
	{
		Acceleration = Acceleration.ProjectOnTo(UpdatedComponent->GetRightVector());
	}
	else
	{
		Acceleration = FVector::ZeroVector;
	}

	if(!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		CalcVelocity(deltaTime, slideFrictionFactor, true, GetMaxBrakingDeceleration());
	}

	iterations++;
	bJustTeleported = false;

	FVector oldLocation = UpdatedComponent->GetComponentLocation();
	FQuat oldRotation = UpdatedComponent->GetComponentRotation().Quaternion();
	FHitResult hit(1.f);
	FVector adjusted = Velocity * deltaTime;
	FVector velPlanDir = FVector::VectorPlaneProject(Velocity, surfaceHit.Normal).GetSafeNormal();
	FQuat newRotation = FRotationMatrix::MakeFromXZ(velPlanDir, surfaceHit.Normal).ToQuat();

	SafeMoveUpdatedComponent(adjusted, newRotation, true, hit);

	if(hit.Time < 1.f)
	{
		HandleImpact(hit, deltaTime, adjusted);
		SlideAlongSurface(adjusted, (1.f - hit.Time), hit.Normal, hit, true);
	}

	FHitResult newSurfaceHit;
	if(!GetSlideSurface(newSurfaceHit) || Velocity.SizeSquared() < pow(minSlideSpeed, 2))
	{
		ExitSlide();
	}

	if(!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - oldLocation) / deltaTime;
	}
}

bool UPlayerCharacterMovementComponent::GetSlideSurface(FHitResult& hit) const
{
	FVector start = UpdatedComponent->GetComponentLocation();
	FVector end = start + CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.f * FVector::DownVector;
	FName profileName = TEXT("BlockAll");
	
	return GetWorld()->LineTraceSingleByProfile(hit, start, end, profileName, parkourCharacterOwner->GetIgnoreCharacterParams());
}

UPlayerCharacterMovementComponent::UPlayerCharacterMovementComponent()
{
	NavAgentProps.bCanCrouch = true;
}

void UPlayerCharacterMovementComponent::CrouchPressed()
{	
	bWantsToCrouch = !bWantsToCrouch;
}

void UPlayerCharacterMovementComponent::SlidePressed()
{
	Safe_bWantsToSlide = !Safe_bWantsToSlide;
}

bool UPlayerCharacterMovementComponent::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

bool UPlayerCharacterMovementComponent::CanAttemptJump() const
{
	return (IsMovingOnGround() || IsFalling()); // Falling included for double-jump and non-zero jump hold time, but validated by character.
}

bool UPlayerCharacterMovementComponent::DoJump(bool bReplayingMoves)
{	
	// Don't jump if we can't move up/down.
	if (!bConstrainToPlane || FMath::Abs(PlaneConstraintNormal.Z) != 1.f)
	{
		Velocity.Z = FMath::Max<FVector::FReal>(Velocity.Z, JumpZVelocity);
		SetMovementMode(MOVE_Falling);
		Safe_bWantsToSlide = false;
		bWantsToCrouch = false;
		return true;
	}	
	return false;
}