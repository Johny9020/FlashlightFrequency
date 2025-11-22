// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/FlashlightComponent.h"
#include "Engine/DataAsset.h"
#include "ColorMappingData.generated.h"

USTRUCT(BlueprintType)
struct FColorMapping
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EFlashlightColor Color;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor AssignedColor;
};

/**
 * 
 */
UCLASS()
class FLASHLIGHTFREQUENCY_API UColorMappingData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FColorMapping> ColorMappings;
	
	UFUNCTION(BlueprintCallable)
	FColorMapping GetColorMapping(EFlashlightColor Color);
};
