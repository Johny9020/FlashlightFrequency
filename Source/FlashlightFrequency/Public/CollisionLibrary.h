// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CollisionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class FLASHLIGHTFREQUENCY_API UCollisionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Collision", Meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm = "ActorsToIgnore", DisplayName = "Multi Cone Trace By Channel", AdvancedDisplay = "TraceColor, TraceHitColor, DrawTime", Keywords = "sweep"))
	static bool ConeTraceMulti(const UObject* WorldContextObject, const FVector& Start, const FRotator& ConeDirection, float ConeHeight, float
	                           ConeHalfAngleDeg, ETraceTypeQuery TraceChannel, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace
	                           ::Type DrawDebugType, TArray<FHitResult>& OutHits, FLinearColor TraceColor = FLinearColor(0.0f, 1.0f, 1.0f), FLinearColor
	                           TraceHitColor = FLinearColor::Green, float DrawTime = 5.0f);
};
