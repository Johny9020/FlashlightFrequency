// Fill out your copyright notice in the Description page of Project Settings.


#include "CollisionLibrary.h"

#include "KismetTraceUtils.h"

bool UCollisionLibrary::ConeTraceMulti(
    const UObject* WorldContextObject,
    const FVector& Start,
    const FRotator& ConeDirection,
    float ConeHeight,
    float ConeHalfAngleDeg,
    ETraceTypeQuery TraceChannel,
    bool bTraceComplex,
    const TArray<AActor*>& ActorsToIgnore,
    EDrawDebugTrace::Type DrawDebugType,
    TArray<FHitResult>& OutHits,
    FLinearColor TraceColor,
    FLinearColor TraceHitColor,
    float DrawTime)
{
    OutHits.Reset();

    // Clamp angle a bit for safety
    const float MaxConeAngleDeg = 85.0f;
    ConeHalfAngleDeg = FMath::Clamp(ConeHalfAngleDeg, 0.0f, MaxConeAngleDeg);

    const float ConeHalfAngleRad = FMath::DegreesToRadians(ConeHalfAngleDeg);
    const float TanHalfAngle     = FMath::Tan(ConeHalfAngleRad);

    ECollisionChannel CollisionChannel = UEngineTypes::ConvertToCollisionChannel(TraceChannel);
    FCollisionQueryParams Params(SCENE_QUERY_STAT(ConeTraceMulti), bTraceComplex);
    Params.bReturnPhysicalMaterial = true;
    Params.AddIgnoredActors(ActorsToIgnore);

    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (!World)
    {
        return false;
    }

    const FVector Axis = ConeDirection.Vector().GetSafeNormal();
    const FVector End  = Start + Axis * ConeHeight;

    const float ConeBaseRadius = ConeHeight * TanHalfAngle;
    const FCollisionShape SphereSweep = FCollisionShape::MakeSphere(ConeBaseRadius);

    TArray<FHitResult> TempHitResults;
    World->SweepMultiByChannel(TempHitResults, Start, End, ConeDirection.Quaternion(), CollisionChannel, SphereSweep, Params);

    OutHits.Reserve(TempHitResults.Num());

    // Small tolerances so edge cases don't flicker
    const float HeightTolerance   = 1.0f;   // units
    const float RadiusToleranceSq = 1.0f;   // units^2

    for (FHitResult& HitResult : TempHitResults)
    {
        // Use Location for sweeps, it's more stable than ImpactPoint
        const FVector HitPos = HitResult.Location;
        const FVector ToHit  = HitPos - Start;

        if (ToHit.IsNearlyZero())
        {
            continue;
        }

        // Distance along cone axis
        const float AxisDist = FVector::DotProduct(ToHit, Axis);

        // Behind the tip or past the end
        if (AxisDist < 0.0f || AxisDist > ConeHeight + HeightTolerance)
        {
            continue;
        }

        // Squared radial distance from axis:
        // |v|^2 = axisDist^2 + radial^2  => radial^2 = |v|^2 - axisDist^2
        const float DistSq = ToHit.SizeSquared();
        float RadialSq     = DistSq - AxisDist * AxisDist;
        RadialSq = FMath::Max(RadialSq, 0.0f); // numeric safety

        // Max radius at this height inside the cone
        const float MaxRadius = AxisDist * TanHalfAngle;
        const float MaxRadiusSq = MaxRadius * MaxRadius;

        if (RadialSq > MaxRadiusSq + RadiusToleranceSq)
        {
            continue;
        }

        // Passed all tests -> inside cone
        OutHits.Add(HitResult);
    }

#if ENABLE_DRAW_DEBUG
    if (DrawDebugType != EDrawDebugTrace::None)
    {
        const bool bPersistent = (DrawDebugType == EDrawDebugTrace::Persistent);

        DrawDebugCone(
            World,
            Start,
            Axis,
            ConeHeight,                  // axis length (not slant)
            ConeHalfAngleRad,
            ConeHalfAngleRad,
            32,
            TraceColor.ToFColor(true),
            bPersistent,
            DrawTime
        );

        DrawDebugSweptSphere(
            World,
            Start,
            End,
            ConeBaseRadius,
            TraceColor.ToFColor(true),
            bPersistent,
            DrawTime
        );

        for (const FHitResult& Hit : OutHits)
        {
            DrawDebugLineTraceSingle(
                World,
                Start,
                Hit.Location,
                DrawDebugType,
                true,
                Hit,
                TraceHitColor,
                TraceHitColor,
                DrawTime
            );
        }
    }
#endif

    return OutHits.Num() > 0;
}

