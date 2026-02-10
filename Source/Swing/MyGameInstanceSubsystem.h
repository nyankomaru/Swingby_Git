// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "MyGameInstanceSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class SWING_API UMyGameInstanceSubsystem : public UGameInstanceSubsystem , public FTickableGameObject
{
	GENERATED_BODY()

public:

	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;

	void Tick(float DeltaTime) override;
	TStatId GetStatId() const override;
};
