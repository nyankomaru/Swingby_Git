// Fill out your copyright notice in the Description page of Project Settings.

#include "ThirdPersonCamera.h"
#include "MyGameInstance.h"
#include "PlayerChara.h"

AThirdPersonCamera::AThirdPersonCamera()
	:m_bRegist(false)
{

}

void AThirdPersonCamera::BeginPlay()
{
	Super::BeginPlay();
}

void AThirdPersonCamera::Tick(float DeltaTime)
{
	//プレイヤーの持つソケットに追加
	if (!m_bRegist)
	{
		GetGameInstance<UMyGameInstance>()->GetPlayer()->AddSocket(this, m_SocketPos);
		m_bRegist = true;
	}
}