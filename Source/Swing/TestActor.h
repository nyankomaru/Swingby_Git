// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyActor.h"
#include "TestActor.generated.h"

/**
 * 
 */
UCLASS()
class SWING_API ATestActor : public AMyActor
{
	GENERATED_BODY()
	
public:
	ATestActor();

protected:
	void BeginPlay() override;

};
