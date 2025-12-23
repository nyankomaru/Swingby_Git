// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

//前方宣言
class APlayerChara;
class USplineComponent;

/**
 * 
 */
UCLASS()
class SWING_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	//プレイヤー設定・取得
	void SetPlayer(APlayerChara* _Player);
	UFUNCTION(BlueprintCallable)
	APlayerChara* GetPlayer() const;

	//コーススプラインの設定・取得
	UFUNCTION(BlueprintCallable)
	void SetCourseSpline(USplineComponent* _Spline);
	USplineComponent* GetCourseSpline() const;
	
public:
	APlayerChara* m_pPlayer;	//プレイヤー
	USplineComponent* m_pCourseSpline;	//コースのスプライン
};
