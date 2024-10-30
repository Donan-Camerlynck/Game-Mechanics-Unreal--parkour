

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "ParkourPlayerCameraManager.generated.h"

UCLASS()
class PARKOUR_API AParkourPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)   float  CrouchBlendDuration = 0.5f;
	float CrouchBlendTime;
	
public:
	AParkourPlayerCameraManager();

	virtual  void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
	
};
