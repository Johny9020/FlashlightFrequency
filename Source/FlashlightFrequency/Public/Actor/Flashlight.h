// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlashlightItem.h"
#include "GameFramework/Actor.h"
#include "Flashlight.generated.h"

class USpotLightComponent;
class UFlashlightComponent;

UCLASS()
class FLASHLIGHTFREQUENCY_API AFlashlight : public AActor
{
	GENERATED_BODY()

public:
	AFlashlight();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Lighting)
	TObjectPtr<USpotLightComponent> Spotlight;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<UFlashlightComponent> FlashlightComponent;
	
	UFUNCTION()
	void FlashlightColorChanged(EFlashlightColor NewColor);
};
