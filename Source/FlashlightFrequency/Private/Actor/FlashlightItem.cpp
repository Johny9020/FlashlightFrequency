// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/Actor/FlashlightItem.h"

// Sets default values
AFlashlightItem::AFlashlightItem()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SetReplicates(true);
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
	
	Mesh->SetCollisionProfileName("BlockAll");
	Mesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void AFlashlightItem::UpdateVisibility(const bool bState) const
{
	Mesh->SetVisibility(bState);
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, bState ? ECR_Block : ECR_Ignore);
}

// Called when the game starts or when spawned
void AFlashlightItem::BeginPlay()
{
	Super::BeginPlay();
	Mesh->SetVisibility(false);
}

