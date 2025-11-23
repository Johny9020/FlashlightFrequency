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
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flashlight")
	EFlashlightColor VisibleWith = EFlashlightColor::Red;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	void UpdateVisibility(bool bState);
	
	UFUNCTION(Server, Reliable)
	void ServerUpdateVisibility(bool bState);

protected:
	virtual void BeginPlay() override;
	
private:	
	UPROPERTY(ReplicatedUsing=OnRep_VisibilityState)
	bool bVisibilityState;
	
	UFUNCTION()
	void OnRep_VisibilityState();

};
