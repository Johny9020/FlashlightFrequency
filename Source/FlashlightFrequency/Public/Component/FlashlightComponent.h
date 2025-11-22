// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ColorTypes.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "FlashlightComponent.generated.h"


class AFlashlightItem;
class AFlashlightFrequencyCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFlashlightColorChangeSignature, EFlashlightColor, NewColor);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FLASHLIGHTFREQUENCY_API UFlashlightComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	explicit UFlashlightComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	FOnFlashlightColorChangeSignature OnFlashlightColorChangeDelegate;

	UPROPERTY(ReplicatedUsing=OnRep_FlashlightColor, BlueprintReadOnly, Category = "Flashlight")
	EFlashlightColor CurrentColor = EFlashlightColor::Red;

	UFUNCTION(BlueprintCallable, Category = "Flashlight")
	void CycleFlashlightColor();

	UFUNCTION(BlueprintCallable, Category = "Flashlight")
	void SetFlashlightColor(EFlashlightColor NewColor);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flashlight")
	float ConeRadius = 30.f; // radius of the sweep

	UFUNCTION(Server, Reliable)
	void Server_ChangeCameraSettings(ACharacter* Actor, bool bCameraYawRotation);

	UPROPERTY(BlueprintReadWrite, Category = "Flashlight")
	FVector LocalFlashlightElbowTargetWS;

	UPROPERTY(BlueprintReadWrite, Category = "Flashlight")
	FVector LocalFlashlightTargetWS;

	UPROPERTY(BlueprintReadWrite, Category = "Flashlight")
	FVector LocalHandEffectorWS;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Flashlight")
	FVector ReplicatedFlashlightElbowTargetWS;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Flashlight")
	FVector ReplicatedFlashlightTargetWS;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Flashlight")
	FVector ReplicatedHandEffectorWS;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flashlight")
	float ArmLength = 55.f;

	UFUNCTION(Server, Reliable)
	void Server_SetPointingFlashlight(bool bState);
	
	void SetPointingFlashlight(bool bState);

	UFUNCTION(Server, Unreliable)
	void Server_SetReplicatedValues(const FVector& TargetWS, const FVector& ElbowWS, const FVector& HandWS);

protected:
	UPROPERTY()
	AFlashlightItem* CurrentRevealedItem = nullptr;

	AFlashlightItem* TraceForItem();

	void HandleLocalTrace();

	UFUNCTION()
	void OnRep_FlashlightColor();

	UFUNCTION(Server, Reliable)
	void Server_SetFlashlightColor(EFlashlightColor NewColor);
	
	void ApplyFlashlightColor(EFlashlightColor NewColor);

	AFlashlightFrequencyCharacter* GetOwnerPawn() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	TEnumAsByte<EDrawDebugTrace::Type> DebugTraceType = EDrawDebugTrace::ForDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	float DebugDrawTime = 2.f;

private:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Actions", meta = (AllowPrivateAccess = "true"))
	bool bPointingFlashlight;
};
