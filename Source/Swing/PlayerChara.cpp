// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerChara.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/KismetMathLibrary.h"
#include "Planet.h"

//コンストラクタ
APlayerChara::APlayerChara()
	: m_MoveDire(0.0f)
	, m_Rot(0.0f)
	, m_ForwardInput(0.0f)
{
	//コリジョンを生成
	m_pMainCollision = CreateDefaultSubobject<UCapsuleComponent>("m_pMainCollision");
	if (m_pMainCollision)
	{
		//ルートに設定
		RootComponent = m_pMainCollision;
		m_pMainCollision->SetSimulatePhysics(false);

		//当たり判定(ここでは全部無視)
		m_pMainCollision->SetCollisionProfileName("AllIgnore");
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

	//移動コンポーネントの生成
	m_pMovement = CreateDefaultSubobject<UFloatingPawnMovement>("m_pMovement");
	if (m_pMovement)
	{
		m_pMovement->Acceleration = 1.0f;
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

//ティック
void APlayerChara::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateRotation(DeltaTime);
	UpdateMove(DeltaTime);

	UE_LOG(LogTemp, Warning, TEXT("%f"), m_MoveDire.Length());
}

//ゲームスタート時
void APlayerChara::BeginPlay()
{
	Super::BeginPlay();

	//当たり判定(ここで判定を入れるとすでに重なっているものもbeginが発生)
	m_pMainCollision->SetCollisionProfileName("MyBlockDynamic");
}

void APlayerChara::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	//相手が重力の場合処理を行う
	if (!OtherComp->ComponentHasTag("Gravity")) { return; }

	//配列に追加
	m_pPlanets.Add(Cast<APlanet>(OtherActor));
	
	//UE_LOG(LogTemp, Warning, TEXT("%f , %f"), Cast<USphereComponent>(OtherComp)->GetScaledSphereRadius(), (OtherComp->GetComponentLocation() - GetActorLocation()).Length());
}

void APlayerChara::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//相手が重力の場合処理を行う
	if (!OtherComp->ComponentHasTag("Gravity")) { return; }

	//配列から削除
	m_pPlanets.Remove(Cast<APlanet>(OtherActor));
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

	//重力を受ける対象分
	for (int i = 0; i < m_pPlanets.Num(); ++i)
	{
		//星に向かう方向
		FVector PlanetDire(m_pPlanets[i]->GetActorLocation() - GetActorLocation());

		//星との距離
		float Distance(PlanetDire.Length());

		//向かう強さ(近いほど設定した値に近くなる)
		float Gravity(m_pPlanets[i]->GetGravity() * (1.0f - Distance / m_pPlanets[i]->GetGradius()));

		m_MoveDire = m_MoveDire + PlanetDire.GetSafeNormal() * Gravity;

		//UE_LOG(LogTemp,Warning, TEXT("%i"), m_pPlanets.Num());
	}

	//AddActorWorldOffset(m_MoveDire * m_ForwardSpeed * DeltaTime);

	AddMovementInput(m_MoveDire,m_MoveDire.Length());
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