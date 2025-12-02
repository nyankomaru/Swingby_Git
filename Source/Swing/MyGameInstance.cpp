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