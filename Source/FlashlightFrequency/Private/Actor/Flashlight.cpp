// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Flashlight.h"

#include "Component/FlashlightComponent.h"
#include "Components/SpotLightComponent.h"


// Sets default values
AFlashlight::AFlashlight()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
	SetRootComponent(Mesh);
	
	Spotlight = CreateDefaultSubobject<USpotLightComponent>("Spotlight");
	Spotlight->SetupAttachment(Mesh);
	Spotlight->SetRelativeLocation(FVector(9.75f, 0.f, 0.f));
	
	/* Spotlight settings */
	Spotlight->SetIntensity(545.2f);
	Spotlight->SetIntensityUnits(ELightUnits::Lumens);
	Spotlight->SetAttenuationRadius(16384.f);
	Spotlight->SetInnerConeAngle(10.5f);
	Spotlight->SetOuterConeAngle(22.5f);
	Spotlight->SetUseTemperature(true);
	Spotlight->SetTemperature(6400.f);
}

// Called when the game starts or when spawned
void AFlashlight::BeginPlay()
{
	Super::BeginPlay();
	
	if (const AActor* Actor = GetAttachParentActor())
	{
		FlashlightComponent = Actor->FindComponentByClass<UFlashlightComponent>();
		if (FlashlightComponent)
		{
			FlashlightComponent->OnFlashlightColorChangeDelegate.AddDynamic(this, &ThisClass::FlashlightColorChanged);
		}
	}
}

