// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/Actor/FlashlightItem.h"

#include "Components/WidgetComponent.h"

// Sets default values
AFlashlightItem::AFlashlightItem()
{
	bReplicates = true;
	AActor::SetReplicateMovement(true);
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionProfileName("BlockAll");
	Mesh->SetGenerateOverlapEvents(true);

	RevealMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RevealMesh"));
	RevealMesh->SetupAttachment(Mesh);
	RevealMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RevealMesh->SetOnlyOwnerSee(false);
	RevealMesh->SetOwnerNoSee(false);
	RevealMesh->SetVisibility(false, true);

	HighlightWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HighlightWidget"));
	HighlightWidget->SetupAttachment(Mesh);
	HighlightWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HighlightWidget->SetDrawAtDesiredSize(true);
	HighlightWidget->SetVisibility(false);
}

// Called when the game starts or when spawned
void AFlashlightItem::BeginPlay()
{
	Super::BeginPlay();

	if (HighlightWidget)
		HighlightWidget->SetVisibility(true);
	if (RevealMesh)
		RevealMesh->SetVisibility(false, true);
}

void AFlashlightItem::SetRevealVisible_Local(const bool bVisible) const
{
	UpdateLocalReveal(bVisible);
}

void AFlashlightItem::UpdateLocalReveal(const bool bVisible) const
{
	if (HighlightWidget)
		HighlightWidget->SetVisibility(bVisible);
	if (RevealMesh)
		RevealMesh->SetVisibility(bVisible, true);
}

void AFlashlightItem::OnPickedUp()
{
	if (HasAuthority())
	{
		Destroy();
	}
}

