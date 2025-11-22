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

	UFUNCTION(BlueprintCallable, Category = "Flashlight")
	void SetRevealVisible_Local(bool bVisible) const;

	UFUNCTION(BlueprintCallable, Category = "Item")
	void OnPickedUp();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UStaticMeshComponent> RevealMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UWidgetComponent> HighlightWidget;

	void UpdateLocalReveal(bool bVisible) const;

};
