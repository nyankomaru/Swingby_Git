// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Tickable.h"
#include "EnhancedInputSubsystems.h"
#include "MyWorldSubsystem.generated.h"

class AMyCamera;

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

	UFUNCTION(BlueprintCallable)
	void SaveOperateTimer();

	//カメラの設定・取得
	UFUNCTION(BlueprintCallable)
	void AddCamera(AMyCamera* _Camera);
	UFUNCTION(BlueprintCallable)
	TArray<AMyCamera*> GetCamera() const;
	//カメラの変更
	UFUNCTION(BlueprintCallable)
	void ChangeCamera(bool _Add);

	//初期位置の設定・取得
	UFUNCTION(BlueprintCallable)
	void SetPlayerInit(FTransform _Form);
	UFUNCTION(BlueprintCallable)
	FTransform GetPlayerInit() const;

private:
	TArray<AMyCamera*> m_pCamera;	//配置されたカメラ
	FTransform m_PlayerRL;	//初期位置
	float m_WorldTimer;	//ワールド開始時点からのタイマー
	float m_OperateTimer;	//ストップウォッチの様なタイマー
	int m_NowCamID;	//現在のカメラ
	bool m_bTimerStart;		//タイマー計測が開始されているか否か
};
