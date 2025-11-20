// Fill out your copyright notice in the Description page of Project Settings.

#include "ArmCamera.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "PlayerChara.h"

AArmCamera::AArmCamera()
	:m_ArmRoot(0.0f)
{
	//ティックの有効化
	PrimaryActorTick.bCanEverTick = true;

	m_pSpringArm = CreateDefaultSubobject<USpringArmComponent>("m_pSpringArm");
	if (m_pSpringArm)
	{
		//アタッチ
		RootComponent = m_pSpringArm;

		GetCameraComponent()->SetupAttachment(RootComponent);
	}
}

//スプリングアームの先の設定
void AArmCamera::SetPoint(AActor* _pActor)
{
	m_pPoint = Cast<APlayerChara>(_pActor);
}

void AArmCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_pPoint)
	{
		SetActorLocation(m_pPoint->GetActorLocation());
		SetActorRotation(m_pPoint->GetUpRotator());
	}
}