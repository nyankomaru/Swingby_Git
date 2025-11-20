// Fill out your copyright notice in the Description page of Project Settings.

#include "Planet.h"
#include "Components/SphereComponent.h"

APlanet::APlanet()
	:m_Gravity(0.0f)
{
	CreateColliAndMesh<USphereComponent, UStaticMeshComponent>();

	if (m_pMainCollision)
	{
		//“®‚©‚¸“–‚½‚é
		m_pMainCollision->SetCollisionProfileName("MyBlockStatic");
	}

	m_pGravitySphere = CreateDefaultSubobject<USphereComponent>("m_pGravitySphere");
	if (m_pGravitySphere)
	{
		m_pGravitySphere->SetupAttachment(RootComponent);
		//“®‚©‚¸d‚È‚é
		m_pGravitySphere->SetCollisionProfileName("MyOverlapStatic");
	}
}

void APlanet::BeginPlay()
{
	if (m_pGravitySphere)
	{
		m_pGravitySphere->SetRelativeScale3D(FVector(m_Gravity * 50.0f));
	}
}

//d—Í‚ÌŽæ“¾
float APlanet::GetGravity() const
{
	return m_Gravity;
}

//d—Í”¼Œa‚ÌŽæ“¾
float APlanet::GetGradius() const
{
	return m_pGravitySphere->GetScaledSphereRadius();
}