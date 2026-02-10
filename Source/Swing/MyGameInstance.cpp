// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameInstance.h"
#include "PlayerChara.h"

//プレイヤー設定・取得
void UMyGameInstance::SetPlayer(APlayerChara* _Player)
{
	m_pPlayer = _Player;
}
APlayerChara* UMyGameInstance::GetPlayer() const
{
	return m_pPlayer;
}

//コーススプラインの設定・取得
void UMyGameInstance::SetCourseSpline(USplineComponent* _Spline)
{
	m_pCourseSpline = _Spline;
}
USplineComponent* UMyGameInstance::GetCourseSpline() const
{
	return m_pCourseSpline;
}

void UMyGameInstance::Tick(float DeltaTime)
{
	//GEngine->AddOnScreenDebugMessage(1, DeltaTime, FColor::Cyan, TEXT("Tick!!"));
}
TStatId UMyGameInstance::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UMyTickableWorldSubsystem, STATGROUP_Tickables);
}