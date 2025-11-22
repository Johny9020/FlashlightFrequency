#pragma once

#include "CoreMinimal.h"
#include "ColorTypes.generated.h"

UENUM(BlueprintType)
enum class EFlashlightColor : uint8
{
	Red UMETA(DisplayName = "Red"),
	Green UMETA(DisplayName = "Green"),
	Blue UMETA(DisplayName = "Blue"),
	UV UMETA(DisplayName = "UV"),
};
