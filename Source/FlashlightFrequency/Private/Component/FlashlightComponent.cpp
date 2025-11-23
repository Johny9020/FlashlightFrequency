#include "Component/FlashlightComponent.h"
#include "FlashlightFrequencyCharacter.h"
#include "Actor/FlashlightItem.h"
#include "Camera/CameraComponent.h"
#include "Character/FlashlightEnemy.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"



UFlashlightComponent::UFlashlightComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
    bAutoActivate = true;
}

void UFlashlightComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UFlashlightComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UFlashlightComponent, CurrentColor);
    DOREPLIFETIME(ThisClass, bPointingFlashlight);

    DOREPLIFETIME_CONDITION(ThisClass, ReplicatedFlashlightTargetWS, COND_SkipOwner);
    DOREPLIFETIME_CONDITION(ThisClass, ReplicatedFlashlightElbowTargetWS, COND_SkipOwner);
    DOREPLIFETIME_CONDITION(ThisClass, ReplicatedHandEffectorWS, COND_SkipOwner);
}

void UFlashlightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    const auto* OwnerPawn = GetOwnerPawn();
    if (!OwnerPawn)
    {
        return;
    }

    if (!OwnerPawn->IsLocallyControlled())
        return;

    if (!bPointingFlashlight)
        return;
    
    HandleLocalTrace();

    const FVector UpperArmLocation = OwnerPawn->GetMesh()->GetBoneLocation(TEXT("upperarm_r"));

    const FVector Right = OwnerPawn->GetActorRightVector();
    const FVector Up = OwnerPawn->GetActorUpVector();

    constexpr float SideOffset = 25.f;
    constexpr float DownOffset = 10.f;

    LocalFlashlightElbowTargetWS = UpperArmLocation + Right * SideOffset - Up * DownOffset;

    Server_SetReplicatedValues(LocalFlashlightTargetWS, LocalFlashlightElbowTargetWS, LocalHandEffectorWS);
}

void UFlashlightComponent::CycleFlashlightColor()
{
    // Client-side request → server authoritative
    EFlashlightColor NextColor;

    switch (CurrentColor)
    {
    case EFlashlightColor::Red:
        NextColor = EFlashlightColor::Green;
        break;
    case EFlashlightColor::Green:
        NextColor = EFlashlightColor::Blue;
        break;
    case EFlashlightColor::Blue:
        NextColor = EFlashlightColor::UV;
        break;
    case EFlashlightColor::UV:
        NextColor = EFlashlightColor::Red;
        break;
    default:
        NextColor = EFlashlightColor::Red;
        break;
    }

    SetFlashlightColor(NextColor);
}

void UFlashlightComponent::SetFlashlightColor(EFlashlightColor NewColor)
{
    APawn* OwnerPawn = GetOwnerPawn();
    if (!OwnerPawn)
    {
        return;
    }
    
    if (OwnerPawn->IsLocallyControlled())
    {
        Server_SetFlashlightColor(NewColor);
        ApplyFlashlightColor(NewColor);
    }
    else if (OwnerPawn->HasAuthority())
    {
        ApplyFlashlightColor(NewColor);
    }
}

AFlashlightFrequencyCharacter* UFlashlightComponent::GetOwnerPawn() const
{
    return Cast<AFlashlightFrequencyCharacter>(GetOwner());
}

void UFlashlightComponent::OnRep_PointingFlashlight() const
{
    ApplyCameraSettings(bPointingFlashlight);
}

void UFlashlightComponent::ServerSetItemVisibilityState_Implementation(AFlashlightItem* Item, bool bState)
{
    if (!Item)
        return;
    
    Item->UpdateVisibility(bState);
}

void UFlashlightComponent::Server_SetFlashlightColor_Implementation(EFlashlightColor NewColor)
{
    ApplyFlashlightColor(NewColor);
}

void UFlashlightComponent::ApplyFlashlightColor(EFlashlightColor NewColor)
{
    if (CurrentColor == NewColor)
    {
        return;
    }

    CurrentColor = NewColor;

    OnFlashlightColorChangeDelegate.Broadcast(NewColor);
    OnRep_FlashlightColor();
}

void UFlashlightComponent::OnRep_FlashlightColor()
{
    // This is called on clients when CurrentColor changes via replication.
    // Hook into Blueprint to change actual light color / material, etc.
    // (You can create a BlueprintImplementableEvent if you want.)
}

/** Client-only: tracing and reveal logic */
void UFlashlightComponent::HandleLocalTrace()
{
    AActor* HitItem = TraceForItem();
    
    if (CurrentHitEnemy && (HitItem != CurrentHitEnemy || HitItem == nullptr))
    {
        CurrentHitEnemy->SetLightReacting(false);
        CurrentHitEnemy = nullptr;
    }
    
    if (CurrentRevealedItem && (HitItem != CurrentRevealedItem || HitItem == nullptr))
    {
        ServerSetItemVisibilityState(CurrentRevealedItem, false);
        CurrentRevealedItem = nullptr;
    }
    
    if (!HitItem)
        return;
    
    if (AFlashlightEnemy* EnemyHit = Cast<AFlashlightEnemy>(HitItem))
    {
        if (!CurrentHitEnemy)
        {
            EnemyHit->SetLightReacting(true);
            CurrentHitEnemy = EnemyHit;
        }else
        {
            if (CurrentHitEnemy != EnemyHit)
            {
                CurrentHitEnemy->SetLightReacting(false);
                EnemyHit->SetLightReacting(true);
                CurrentHitEnemy = EnemyHit;
            }
        }
    }
    
    if (AFlashlightItem* Item = Cast<AFlashlightItem>(HitItem))
    {
        if (Item)
        {
            // Check color match on client
            if (Item->VisibleWith == CurrentColor)
            {
                ServerSetItemVisibilityState(Item, true);
                CurrentRevealedItem = Item;
            }
            else
            {
                ServerSetItemVisibilityState(Item, false);
                CurrentRevealedItem = nullptr;
            }
        }   
    }
}

void UFlashlightComponent::Server_SetPointingFlashlight_Implementation(bool bState)
{
    bPointingFlashlight = bState;
    ApplyCameraSettings(bState);
}

void UFlashlightComponent::SetPointingFlashlight(bool bState)
{
    if (!GetOwnerPawn()) return;
    
    if (!GetOwnerPawn()->HasAuthority())
    {
        Server_SetPointingFlashlight(bState);
    }
    
    if (!bState)
    {
        if (CurrentHitEnemy)
        {
            CurrentHitEnemy->SetLightReacting(false);
            CurrentHitEnemy = nullptr;
        }
        
        if (CurrentRevealedItem)
        {
            ServerSetItemVisibilityState(CurrentRevealedItem, false);
            CurrentRevealedItem = nullptr;
        }
    }
    
    ApplyCameraSettings(bState);
    bPointingFlashlight = bState;
}

void UFlashlightComponent::ApplyCameraSettings(const bool bState) const
{
    auto* Owner = GetOwnerPawn();
    if (!Owner) return;
    
    Owner->bUseControllerRotationPitch = false;
    Owner->bUseControllerRotationYaw   = bState;
    Owner->bUseControllerRotationRoll  = false;

    Owner->GetCharacterMovement()->bOrientRotationToMovement = !bState;
}


void UFlashlightComponent::Server_SetReplicatedValues_Implementation(const FVector& TargetWS, const FVector& ElbowWS, const FVector& HandWS)
{
    ReplicatedFlashlightTargetWS = TargetWS;
    ReplicatedFlashlightElbowTargetWS = ElbowWS;
    ReplicatedHandEffectorWS = HandWS;
}

AActor* UFlashlightComponent::TraceForItem()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return nullptr;
    }

    auto* OwnerPawn = GetOwnerPawn();
    if (!OwnerPawn)
    {
        return nullptr;
    }

    // -------- Get Start + Direction --------
    const FVector Start = OwnerPawn->GetCapsuleComponent()->GetComponentLocation();
    const FRotator ComponentRotation = OwnerPawn->GetFollowCamera()->GetComponentRotation();
    const FRotator DirRot = FRotator(ComponentRotation.Pitch + 5, ComponentRotation.Yaw, ComponentRotation.Roll);
    const FVector End = Start + (DirRot.Quaternion().GetForwardVector() * 800.f);

    LocalFlashlightTargetWS = End;

    const FVector ShoulderWS = OwnerPawn->GetMesh()->GetBoneLocation(TEXT("upperarm_r"));
    const FVector AimDir = (End - ShoulderWS).GetSafeNormal();

    
    LocalHandEffectorWS = ShoulderWS + AimDir * ArmLength;

    TArray<FHitResult> Hits;
    TArray<AActor*> HitActors;
    HitActors.Add(OwnerPawn);

    const ETraceTypeQuery FlashlightTrace = UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1);

    UKismetSystemLibrary::SphereTraceMulti(this, Start, End, ConeRadius, FlashlightTrace, true, HitActors, DebugTraceType, Hits, true, FColor::Red, FColor::Green, DebugDrawTime);
    

    // -------- Pick best AFlashlightItem in the cone --------
    AActor* BestItem = nullptr;
    float BestDistSq = TNumericLimits<float>::Max();

    for (const FHitResult& Hit : Hits)
    {
        AActor* HitActor = Hit.GetActor();
        if (!HitActor)
        {
            continue;
        }

        if (const float DistSq = (Hit.ImpactPoint - Start).SizeSquared(); DistSq < BestDistSq)
        {
            BestDistSq = DistSq;
            BestItem   = HitActor;
        }
    }

    return BestItem;
}






