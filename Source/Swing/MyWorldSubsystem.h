// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Tickable.h"
#include "MyWorldSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class SWING_API UMyWorldSubsystem : public UWorldSubsystem , public FTickableGameObject
{
	GENERATED_BODY()
	
public:
	UMyWorldSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	//タイマー計測状態の切替
	UFUNCTION(BlueprintCallable)
	void SwitchTimer();

	UFUNCTION(BlueprintCallable)
	float GetWorldTimer()const;

	UFUNCTION(BlueprintCallable)
	float GetOperateTimer()const;

private:
	float m_WorldTimer;	//ワールド開始時点からのタイマー
	float m_OperateTimer;	//ストップウォッチの様なタイマー

	bool m_bTimerStart;		//タイマー計測が開始されているか否か
};
