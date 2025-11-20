// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerChara.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Planet.h"

APlayerChara::APlayerChara()
	:m_Rot(0.0f)
	, m_pPlanet(NULL)
	, m_ForwardInput(0.0f)
	, m_MoveDire(0.0f)
{
	//コリジョンを生成
	m_pMainCollision = CreateDefaultSubobject<UCapsuleComponent>("m_pMainCollision");
	if (m_pMainCollision)
	{
		//ルートに設定
		RootComponent = m_pMainCollision;
		m_pMainCollision->SetSimulatePhysics(false);

		//当たり判定
		m_pMainCollision->SetCollisionProfileName("MyBlockDynamic");
		//重力無効
		m_pMainCollision->SetEnableGravity(false);
		//オーバーラップイベントの有効化・登録
		m_pMainCollision->SetGenerateOverlapEvents(true);
		m_pMainCollision->OnComponentBeginOverlap.AddDynamic(this, &APlayerChara::OnOverlapBegin);
		m_pMainCollision->OnComponentEndOverlap.AddDynamic(this, &APlayerChara::OnOverlapEnd);
	}

	//メッシュを生成
	m_pMesh = CreateDefaultSubobject<UStaticMeshComponent>("m_pMesh");
	if (m_pMesh)
	{
		//ルート（コリジョン）にアタッチ
		m_pMesh->SetupAttachment(RootComponent);
		//メッシュは当たり判定無し
		m_pMesh->SetCollisionProfileName("AllIgnore");
	}

	//スプリングアームの生成
	m_pSpringArm = CreateDefaultSubobject<USpringArmComponent>("m_pSpringArm");
	if (m_pSpringArm)
	{
		//アタッチ
		m_pSpringArm->SetupAttachment(RootComponent);
	}

	//カメラ生成
	m_pCamera = CreateDefaultSubobject<UCameraComponent>("m_pCamera");
	if (m_pCamera)
	{
		//アタッチ
		m_pCamera->SetupAttachment(m_pSpringArm);
	}

	//UE_LOG(LogTemp, Warning, TEXT("%f"), m_Rot.Pitch);
}

//入力の関連付け
void APlayerChara::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("RightTrigger", this, &APlayerChara::MoveForward);
	InputComponent->BindAxis("LeftTrigger", this, &APlayerChara::MoveForward);
	InputComponent->BindAxis("W_Key", this, &APlayerChara::MoveForwardKey);
	InputComponent->BindAxis("S_Key", this, &APlayerChara::MoveForwardKey);
	InputComponent->BindAxis("LeftStickX", this, &APlayerChara::RotYaw);
	InputComponent->BindAxis("MouseX", this, &APlayerChara::RotYawMouse);
	InputComponent->BindAxis("LeftStickY", this, &APlayerChara::RotPitch);
	InputComponent->BindAxis("MouseY", this, &APlayerChara::RotPitchMouse);
	InputComponent->BindAxis("RightShoulder", this, &APlayerChara::RotRoll);
	InputComponent->BindAxis("LeftShoulder", this, &APlayerChara::RotRoll);
	InputComponent->BindAxis("RightClick", this, &APlayerChara::RotRoll);
	InputComponent->BindAxis("LeftClick", this, &APlayerChara::RotRoll);
}

void APlayerChara::Tick(float DeltaTime)
{
	UpdateRotation(DeltaTime);
	UpdateMove(DeltaTime);
}

void APlayerChara::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("%f , %f"), Cast<USphereComponent>(OtherComp)->GetScaledSphereRadius(), (OtherComp->GetComponentLocation() - GetActorLocation()).Length());

	if (!m_pPlanet)
	{
		m_pPlanet = Cast<APlanet>(OtherActor);
	}
}

void APlayerChara::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (m_pPlanet)
	{
		m_pPlanet = NULL;
	}
}

//上方向の向きの取得
FRotator APlayerChara::GetUpRotator()
{
	return UKismetMathLibrary::FindLookAtRotation(FVector(0.0f), GetActorUpVector());
}

//回転の更新
void APlayerChara::UpdateRotation(float DeltaTime)
{
	if (!m_Rot.IsZero())
	{
		AddActorLocalRotation(m_Rot * DeltaTime * m_RotSpeedRate);

		m_Rot = FRotator(0.0f, 0.0f, 0.0f);
	}
}

//移動の更新
void APlayerChara::UpdateMove(float DeltaTime)
{
	//前進入力のある時
	if (m_ForwardInput != 0.0f)
	{
		m_MoveDire = m_MoveDire + m_pMesh->GetUpVector() * m_ForwardInput * m_ForwardSpeed;

		//次の入力の為にリセット
		m_ForwardInput = 0.0f;
	}

	//重力を受けているとき
	if (m_pPlanet)
	{
		//星に向かう方向
		FVector PlanetDire(m_pPlanet->GetActorLocation() - GetActorLocation());

		//星との距離
		float Distance(PlanetDire.Length());

		//向かう強さ(近いほど設定した値に近くなる)
		float Gravity(m_pPlanet->GetGravity() * (1.0f - Distance / m_pPlanet->GetGradius()));

		m_MoveDire = m_MoveDire + PlanetDire.GetSafeNormal() * Gravity;
	}

	AddActorWorldOffset(m_MoveDire * m_ForwardSpeed * DeltaTime);
}

//移動
void APlayerChara::MoveForward(float _value)
{
	if (_value != 0.0f)
	{
		m_ForwardInput = _value;
	}
}
void APlayerChara::MoveForwardKey(float _value)
{
	if (_value != 0.0f)
	{
		m_ForwardInput = _value;
	}
}

//回転
void APlayerChara::RotYaw(float _value)
{
	if (_value != 0.0f)
	{
		m_Rot.Roll = _value * 0.5f;
	}
}
void APlayerChara::RotPitch(float _value)
{
	if (_value != 0.0f)
	{
		m_Rot.Pitch = _value * 0.5f;
	}
}
void APlayerChara::RotYawMouse(float _value)
{
	if (_value != 0.0f)
	{
		m_Rot.Roll = _value * 0.5f;
	}
}
void APlayerChara::RotPitchMouse(float _value)
{
	if (_value != 0.0f)
	{
		m_Rot.Pitch = _value * 0.5f;
	}
}
void APlayerChara::RotRoll(float _value)
{
	if (_value != 0.0f)
	{
		m_Rot.Yaw = _value * 0.5f;
	}
}