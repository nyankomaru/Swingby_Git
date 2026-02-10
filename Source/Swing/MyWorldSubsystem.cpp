// Fill out your copyright notice in the Description page of Project Settings.


#include "MyWorldSubsystem.h"
#include "ThirdPersonCamera.h"

UMyWorldSubsystem::UMyWorldSubsystem()
	: m_WorldTimer(0.0f)
	, m_OperateTimer(0.0f)
	, m_bTimerStart(false)
{

}

void UMyWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}
void UMyWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UMyWorldSubsystem::Tick(float DeltaTime)
{
	m_WorldTimer += DeltaTime;

	if (m_bTimerStart)
	{
		m_OperateTimer += DeltaTime;
	}

	//GEngine->AddOnScreenDebugMessage(0, DeltaTime, FColor::White,FString::SanitizeFloat(m_WorldTimer));
	//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::White,FString::SanitizeFloat(m_OperateTimer));
}
TStatId UMyWorldSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UMyTickableWorldSubsystem, STATGROUP_Tickables);
}

//タイマー計測状態の切替
void UMyWorldSubsystem::SwitchTimer()
{
	m_bTimerStart = !m_bTimerStart;
}

float UMyWorldSubsystem::GetWorldTimer()const
{
	return m_WorldTimer;
}

float UMyWorldSubsystem::GetOperateTimer()const
{
	return m_OperateTimer;
}

//カメラの設定・取得
void UMyWorldSubsystem::AddThirdCamera(AThirdPersonCamera* _Camera)
{
	m_pThirdCamera.Push(_Camera);
}
TArray<AThirdPersonCamera*> UMyWorldSubsystem::GetThirdCamera() const
{
	return m_pThirdCamera;
}