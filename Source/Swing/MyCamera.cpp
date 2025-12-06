// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCamera.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "MyGameInstance.h"

// Sets default values
AMyCamera::AMyCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//アームの生成
	m_pSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("m_pSpringArm"));
	if (m_pSpringArm)
	{
		//ルートに接続
		m_pSpringArm->SetupAttachment(RootComponent);
	}

	//カメラの生成
	m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("m_pCamera"));
	if (m_pCamera)
	{
		//アームに接続
		m_pCamera->SetupAttachment(m_pSpringArm);
	}

}

// Called when the game starts or when spawned
void AMyCamera::BeginPlay()
{
	Super::BeginPlay();

	//ゲームインスタンスに登録
	GetGameInstance<UMyGameInstance>()->GetPlayer();
	
}

// Called every frame
void AMyCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

