// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FlashlightEnemy.h"

#include "AI/FlashlightAIController.h"


// Sets default values
AFlashlightEnemy::AFlashlightEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AFlashlightEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	AIController = Cast<AFlashlightAIController>(NewController);
}

// Called when the game starts or when spawned
void AFlashlightEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

