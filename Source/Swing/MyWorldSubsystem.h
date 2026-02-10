// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Tickable.h"
#include "MyWorldSubsystem.generated.h"

class AThirdPersonCamera;

UCLASS()
class SWING_API UMyWorldSubsystem : public UWorldSubsystem , public FTickableGameObject
{
	GENERATED_BODY()
	
public:
	UMyWorldSubsystem();

	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;

	void Tick(float DeltaTime) override;
	TStatId GetStatId() const override;

	//タイマー計測状態の切替
	UFUNCTION(BlueprintCallable)
	void SwitchTimer();

	UFUNCTION(BlueprintCallable)
	float GetWorldTimer()const;

	UFUNCTION(BlueprintCallable)
	float GetOperateTimer()const;

	//カメラの設定・取得
	UFUNCTION(BlueprintCallable)
	void AddThirdCamera(AThirdPersonCamera* _Camera);
	UFUNCTION(BlueprintCallable)
	TArray<AThirdPersonCamera*> GetThirdCamera() const;

private:
	TArray<AThirdPersonCamera*> m_pThirdCamera;	//配置されたカメラ
	float m_WorldTimer;	//ワールド開始時点からのタイマー
	float m_OperateTimer;	//ストップウォッチの様なタイマー

	bool m_bTimerStart;		//タイマー計測が開始されているか否か
};
