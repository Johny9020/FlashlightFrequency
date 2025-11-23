// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/FlashlightComponent.h"
#include "GameFramework/Actor.h"
#include "FlashlightItem.generated.h"

class UWidgetComponent;

UCLASS()
class FLASHLIGHTFREQUENCY_API AFlashlightItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AFlashlightItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flashlight")
	EFlashlightColor VisibleWith = EFlashlightColor::Red;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	void UpdateVisibility(bool bState) const;

protected:
	virtual void BeginPlay() override;

};
