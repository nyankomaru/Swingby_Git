// Fill out your copyright notice in the Description page of Project Settings.

#include "Planet.h"
#include "Components/SphereComponent.h"

APlanet::APlanet()
	:m_Gravity(0.0f)
{
	//コリジョン（惑星自体）とメッシュの生成
	CreateColliAndMesh<USphereComponent, UStaticMeshComponent>();
	if (m_pMainCollision)
	{
		//動かず当たる
		m_pMainCollision->SetCollisionProfileName("MyBlockStatic");
	}

	//重力範囲の生成
	m_pGravitySphere = CreateDefaultSubobject<USphereComponent>("m_pGravitySphere");
	if (m_pGravitySphere)
	{
		m_pGravitySphere->SetupAttachment(RootComponent);
		//動かず重なる
		m_pGravitySphere->SetCollisionProfileName("MyOverlapStatic");
		//タグを持たせる
		m_pGravitySphere->ComponentTags.Add("Gravity");
		//エディタ上でのちらつきを抑える為に小さくする
		m_pGravitySphere->SetWorldScale3D(FVector(0.2f));
	}
}

void APlanet::BeginPlay()
{
	if (m_pGravitySphere)
	{
		m_pGravitySphere->SetRelativeScale3D(FVector(m_Gravity * 200.0f));
	}
}

//重力の取得
float APlanet::GetGravity() const
{
	return m_Gravity;
}

//重力半径の取得
float APlanet::GetGradius() const
{
	return m_pGravitySphere->GetScaledSphereRadius();
}

//名前の取得
FName APlanet::GetName() const
{
	return m_Name;
}