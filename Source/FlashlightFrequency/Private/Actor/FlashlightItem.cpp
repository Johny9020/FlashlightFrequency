// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/Actor/FlashlightItem.h"

#include "Net/UnrealNetwork.h"

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

void AFlashlightItem::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, bVisibilityState);
}

void AFlashlightItem::UpdateVisibility(const bool bState)
{
	if (!HasAuthority())
	{
		ServerUpdateVisibility(bState);
		return;
	}
	
	bVisibilityState = bState;
	OnRep_VisibilityState();
}

// Called when the game starts or when spawned
void AFlashlightItem::BeginPlay()
{
	Super::BeginPlay();
	Mesh->SetVisibility(false);
}

void AFlashlightItem::OnRep_VisibilityState()
{
	Mesh->SetVisibility(bVisibilityState);
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, !bVisibilityState ? ECR_Block : ECR_Ignore);
}

void AFlashlightItem::ServerUpdateVisibility_Implementation(bool bState)
{
	UpdateVisibility(bState);
}

