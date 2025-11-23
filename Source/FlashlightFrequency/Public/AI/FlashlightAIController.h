// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "FlashlightAIController.generated.h"

class UBehaviorTreeComponent;
/**
 * 
 */
UCLASS()
class FLASHLIGHTFREQUENCY_API AFlashlightAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AFlashlightAIController();
	
protected:	
	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
};
