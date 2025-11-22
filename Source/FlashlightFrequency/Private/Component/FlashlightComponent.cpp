#include "Component/FlashlightComponent.h"
#include "FlashlightFrequencyCharacter.h"
#include "Actor/FlashlightItem.h"
#include "Camera/CameraComponent.h"
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

    const float SideOffset = 25.f;
    const float DownOffset = 10.f;

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

    // Clients call server to change color
    if (OwnerPawn->IsLocallyControlled())
    {
        Server_SetFlashlightColor(NewColor);

        // Optional: apply instantly for local responsiveness
        ApplyFlashlightColor(NewColor);
    }
    else if (GetOwnerRole() == ROLE_Authority)
    {
        // In case the server wants to force-set it
        ApplyFlashlightColor(NewColor);
    }

    GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Emerald, FString::Printf(TEXT("Flashlight Color: %hhd"), NewColor));
}

AFlashlightFrequencyCharacter* UFlashlightComponent::GetOwnerPawn() const
{
    return Cast<AFlashlightFrequencyCharacter>(GetOwner());
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
    AFlashlightItem* HitItem = TraceForItem();

    // If we had an item revealed and it's no longer valid or not hit, hide it
    if (CurrentRevealedItem && CurrentRevealedItem != HitItem)
    {
        CurrentRevealedItem->SetRevealVisible_Local(false);
        CurrentRevealedItem = nullptr;
    }

    if (HitItem)
    {
        // Check color match on client
        if (HitItem->VisibleWith == CurrentColor)
        {
            // Reveal for this client only
            HitItem->SetRevealVisible_Local(true);
            CurrentRevealedItem = HitItem;
        }
        else
        {
            // No match, ensure it's hidden locally
            HitItem->SetRevealVisible_Local(false);
            CurrentRevealedItem = nullptr;
        }
    }
}

void UFlashlightComponent::Server_SetPointingFlashlight_Implementation(bool bState)
{
    SetPointingFlashlight(bState);
}

void UFlashlightComponent::SetPointingFlashlight(bool bState)
{
    auto* Owner = GetOwnerPawn();
    if (bState)
    {
        Owner->bUseControllerRotationPitch = false;
        Owner->bUseControllerRotationYaw = true;
        Owner->bUseControllerRotationRoll = false;
        Owner->GetCharacterMovement()->bOrientRotationToMovement = false;
        
        if (!Owner->HasAuthority())
        {
            Server_ChangeCameraSettings(Owner, bState);
        }
    }else
    {
        Owner->bUseControllerRotationPitch = false;
        Owner->bUseControllerRotationYaw = false;
        Owner->bUseControllerRotationRoll = false;
        
        Owner->GetCharacterMovement()->bOrientRotationToMovement = true;
        
        if (!Owner->HasAuthority())
        {
            Server_ChangeCameraSettings(Owner, bState);
        }

        if (CurrentRevealedItem)
        {
            CurrentRevealedItem->SetRevealVisible_Local(false);
            CurrentRevealedItem = nullptr;
        }
    }
    
    bPointingFlashlight = bState;
}

void UFlashlightComponent::Server_SetReplicatedValues_Implementation(const FVector& TargetWS, const FVector& ElbowWS, const FVector& HandWS)
{
    ReplicatedFlashlightTargetWS = TargetWS;
    ReplicatedFlashlightElbowTargetWS = ElbowWS;
    ReplicatedHandEffectorWS = HandWS;
}

void UFlashlightComponent::Server_ChangeCameraSettings_Implementation(ACharacter* Actor, bool bCameraYawRotation)
{
    Actor->bUseControllerRotationPitch = !bCameraYawRotation;
    Actor->bUseControllerRotationYaw = bCameraYawRotation;
    Actor->bUseControllerRotationRoll = !bCameraYawRotation;
    
    Actor->GetCharacterMovement()->bOrientRotationToMovement = !bCameraYawRotation;
}

AFlashlightItem* UFlashlightComponent::TraceForItem()
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
    FVector Start = OwnerPawn->GetCapsuleComponent()->GetComponentLocation();
    FRotator ComponentRotation = OwnerPawn->GetFollowCamera()->GetComponentRotation();
    FRotator DirRot = FRotator(ComponentRotation.Pitch + 5, ComponentRotation.Yaw, ComponentRotation.Roll);
    FVector End = Start + (DirRot.Quaternion().GetForwardVector() * 800.f);

    LocalFlashlightTargetWS = End;

    FVector ShoulderWS = OwnerPawn->GetMesh()->GetBoneLocation(TEXT("upperarm_r"));
    FVector AimDir = (End - ShoulderWS).GetSafeNormal();

    
    LocalHandEffectorWS = ShoulderWS + AimDir * ArmLength;

    TArray<FHitResult> Hits;
    TArray<AActor*> HitActors;
    HitActors.Add(OwnerPawn);

    ETraceTypeQuery FlashlightTrace = UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1);

    bool bAnyHits = UKismetSystemLibrary::SphereTraceMulti(this, Start, End, ConeRadius, FlashlightTrace, true, HitActors, DebugTraceType, Hits, true, FColor::Red, FColor::Green, DebugDrawTime);
    

    // -------- Pick best AFlashlightItem in the cone --------
    AFlashlightItem* BestItem = nullptr;
    float BestDistSq = TNumericLimits<float>::Max();

    for (const FHitResult& Hit : Hits)
    {
        AActor* HitActor = Hit.GetActor();
        if (!HitActor)
        {
            continue;
        }
        
        AFlashlightItem* Item = Cast<AFlashlightItem>(HitActor);
        if (!Item)
        {
            continue;
        }

        const float DistSq = (Hit.ImpactPoint - Start).SizeSquared();
        if (DistSq < BestDistSq)
        {
            BestDistSq = DistSq;
            BestItem   = Item;
        }
    }

    return BestItem;
}






