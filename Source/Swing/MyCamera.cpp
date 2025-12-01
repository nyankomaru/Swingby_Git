// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCamera.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
AMyCamera::AMyCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//アームの生成
	m_pSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("m_pSpringArm"));
	if (m_pSpringArm)
	{
		//ルートに設定
		RootComponent = m_pSpringArm;
	}

	//カメラの生成
	m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("m_pCamera"));
	if (m_pCamera)
	{
		m_pCamera->SetupAttachment(RootComponent);
	}

}

// Called when the game starts or when spawned
void AMyCamera::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

