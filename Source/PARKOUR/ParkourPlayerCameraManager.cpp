#include "ParkourPlayerCameraManager.h"
#include "ParkourCharacter.h"
#include "PlayerCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

AParkourPlayerCameraManager::AParkourPlayerCameraManager()
{
	
}

void AParkourPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);
	
	if(AParkourCharacter* ParkourCharacter = Cast<AParkourCharacter>(GetOwningPlayerController()->GetPawn()))
	{
		UPlayerCharacterMovementComponent* pParkourCharacterMovementComponent = ParkourCharacter->GetPlayerCharacterMovement();
		FVector TargetCrouchOffset = FVector(0,0, pParkourCharacterMovementComponent->GetCrouchedHalfHeight() - ParkourCharacter->GetClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		FVector Offset = FMath::Lerp(FVector::ZeroVector, TargetCrouchOffset, FMath::Clamp(CrouchBlendTime / CrouchBlendDuration, 0.f,  1.f));

		if(pParkourCharacterMovementComponent->IsCrouching())
		{
			CrouchBlendTime = FMath::Clamp(CrouchBlendTime + DeltaTime, 0.f, CrouchBlendDuration);
			Offset -= TargetCrouchOffset;
		}
		else
		{
			CrouchBlendTime = FMath::Clamp(CrouchBlendTime - DeltaTime, 0.f, CrouchBlendDuration);
		}

		if(pParkourCharacterMovementComponent->IsMovingOnGround() || pParkourCharacterMovementComponent->MovementMode == MOVE_Falling)
		{
			OutVT.POV.Location += Offset;
		}
	}
}
