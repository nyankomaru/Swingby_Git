// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Tickable.h"
#include "MyWorldSubsystem.generated.h"

class AMyCamera;
class UUserWidget;

UCLASS()
class SWING_API UMyWorldSubsystem : public UWorldSubsystem , public FTickableGameObject /*,public APlayerController*/
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
	void AddCamera(AMyCamera* _Camera);
	UFUNCTION(BlueprintCallable)
	TArray<AMyCamera*> GetCamera() const;

	//ウィジェットの設定
	UFUNCTION(BlueprintCallable)
	void AddWidget(UUserWidget* _Widget);
	UFUNCTION(BlueprintCallable)
	TArray<UUserWidget*> GetWidgeta() const;

private:
	TArray<AMyCamera*> m_pCamera;	//配置されたカメラ
	TArray<UUserWidget*> m_pWidget;	//ウィジェット
	float m_WorldTimer;	//ワールド開始時点からのタイマー
	float m_OperateTimer;	//ストップウォッチの様なタイマー

	bool m_bTimerStart;		//タイマー計測が開始されているか否か
};
