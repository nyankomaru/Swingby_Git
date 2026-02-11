// Fill out your copyright notice in the Description page of Project Settings.


#include "MyWorldSubsystem.h"
#include "MyCamera.h"
#include "MyCalcu.h"

UMyWorldSubsystem::UMyWorldSubsystem()
	: m_WorldTimer(0.0f)
	, m_OperateTimer(0.0f)
	, m_bTimerStart(false)
{

}

void UMyWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	m_WorldTimer = 0.0f;
	m_OperateTimer = 0.0f;
	m_NowCamID = 0;
	m_bTimerStart = false;
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
void UMyWorldSubsystem::AddCamera(AMyCamera* _Camera)
{
	m_pCamera.Push(_Camera);
}
TArray<AMyCamera*> UMyWorldSubsystem::GetCamera() const
{
	return m_pCamera;
}
//カメラの変更
void UMyWorldSubsystem::ChangeCamera(bool _Add)
{
	if (m_pCamera.Num() != 0)
	{
		m_NowCamID = MyCalcu::Clamp(m_NowCamID + ((_Add) ? 1 : -1), 0, m_pCamera.Num() - 1);
		GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(m_pCamera[m_NowCamID],1.0f);
	}
}

//初期位置の設定・取得
void UMyWorldSubsystem::SetPlayerInit(FTransform _Form)
{
	m_PlayerRL = _Form;
}
FTransform UMyWorldSubsystem::GetPlayerInit() const
{
	return m_PlayerRL;
}