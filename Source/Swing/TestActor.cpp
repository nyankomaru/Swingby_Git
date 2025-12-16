// Fill out your copyright notice in the Description page of Project Settings.

#include "TestActor.h"
#include "Components/BoxComponent.h"

ATestActor::ATestActor()
{
	CreateColliAndMesh<UBoxComponent, UStaticMeshComponent>();
}

void ATestActor::BeginPlay()
{
	Super::BeginPlay();

	m_pMainCollision->SetSimulatePhysics(true);
}