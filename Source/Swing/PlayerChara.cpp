// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerChara.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/KismetMathLibrary.h"
#include "Planet.h"
#include "DrawDebugHelpers.h"
#include "MyGameInstance.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

//コンストラクタ
APlayerChara::APlayerChara()
	: m_MoveDire(0.0f)
	, m_Rot(0.0f)
	, m_CameraRot(0.0f)
	, m_CameraRotInput(0.0f)
	, m_ForwardInput(0.0f)
	, m_ChangeCtrl(0.0f)
	, m_Speed(0.0f)
{

	//ルートを生成
	//m_pSceneComp = CreateDefaultSubobject<USceneComponent>("m_pSceneComp");
	//if (m_pSceneComp)
	//{
	//	//これをルートとして他をまとめて操作
	//	RootComponent = m_pSceneComp;
	//}
	//メッシュを生成
	m_pMesh = CreateDefaultSubobject<UStaticMeshComponent>("m_pMesh");
	if (m_pMesh)
	{
		//ルート（コリジョン）にアタッチ
		//m_pMesh->SetupAttachment(RootComponent);
		RootComponent = m_pMesh;
		//オーバーラップイベントの有効化・登録
		//メッシュのコリジョンで判定を行う
		m_pMesh->SetGenerateOverlapEvents(true);
		m_pMesh->OnComponentBeginOverlap.AddDynamic(this, &APlayerChara::OnOverlapBegin);
		m_pMesh->OnComponentEndOverlap.AddDynamic(this, &APlayerChara::OnOverlapEnd);
		//当たり判定(ここでは全部無視)
		m_pMesh->SetCollisionProfileName("AllIgnore");
		//重力の影響は受けない
		m_pMesh->SetEnableGravity(false);
	}
	//スプリングアームの生成
	m_pSpring = CreateDefaultSubobject<USpringArmComponent>("m_pSpring");
	if (m_pSpring)
	{
		//ルート（コリジョン）にアタッチ
		m_pSpring->SetupAttachment(RootComponent);
	}
	//カメラの生成
	m_pCamera = CreateDefaultSubobject<UCameraComponent>("m_pCamera");
	if (m_pCamera)
	{
		//アームにくっつける
		m_pCamera->SetupAttachment(m_pSpring);
	}
	//移動コンポーネントの生成
	m_pMovement = CreateDefaultSubobject<UFloatingPawnMovement>("m_pMovement");
	if (m_pMovement)
	{

	}

	//UE_LOG(LogTemp, Warning, TEXT("%f"), m_Rot.Pitch);
}

//入力の関連付け
void APlayerChara::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//前進・後退
	InputComponent->BindAxis("RightTrigger", this, &APlayerChara::MoveForward);
	InputComponent->BindAxis("LeftTrigger", this, &APlayerChara::MoveForward);
	InputComponent->BindAxis("Space", this, &APlayerChara::MoveForward);
	InputComponent->BindAxis("Right_Ctrl", this, &APlayerChara::MoveForward);
	//回転
	InputComponent->BindAxis("LeftStickY", this, &APlayerChara::RotPitch);
	InputComponent->BindAxis("W_Key", this, &APlayerChara::RotPitch);
	InputComponent->BindAxis("S_Key", this, &APlayerChara::RotPitch);
	InputComponent->BindAxis("LeftStickX", this, &APlayerChara::RotYaw);
	InputComponent->BindAxis("A_Key", this, &APlayerChara::RotYaw);
	InputComponent->BindAxis("D_Key", this, &APlayerChara::RotYaw);
	InputComponent->BindAxis("RightShoulder", this, &APlayerChara::RotRoll);
	InputComponent->BindAxis("LeftShoulder", this, &APlayerChara::RotRoll);
	InputComponent->BindAxis("E_Key", this, &APlayerChara::RotRoll);
	InputComponent->BindAxis("Q_Key", this, &APlayerChara::RotRoll);
	//カメラ回転
	InputComponent->BindAxis("RightStickX", this, &APlayerChara::CameraRotPitch);
	InputComponent->BindAxis("Right_Key", this, &APlayerChara::CameraRotPitch);
	InputComponent->BindAxis("Left_Key", this, &APlayerChara::CameraRotPitch);
	InputComponent->BindAxis("RightStickY", this, &APlayerChara::CameraRotYaw);
	InputComponent->BindAxis("Up_Key", this, &APlayerChara::CameraRotYaw);
	InputComponent->BindAxis("Down_Key", this, &APlayerChara::CameraRotYaw);
	//操作変更
	InputComponent->BindAxis("Left_Ctrl", this, &APlayerChara::ChangeCtrl);
	//コリジョン変更
	InputComponent->BindAction("O_Key", EInputEvent::IE_Pressed, this, &APlayerChara::ChangeCollision);
}

//ティック
void APlayerChara::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateRotation(DeltaTime);
	UpdateMove(DeltaTime);
	UpdateCamera(DeltaTime);

	//UE_LOG(LogTemp, Warning, TEXT("%f"), m_pMovement->Acceleration);

	//進行方向に線を表示
	FVector AcLoc(GetActorLocation());
	DrawDebugLine(GetWorld(), AcLoc, AcLoc + m_pMovement->Velocity, FColor::Green);
}

//ゲームスタート時
void APlayerChara::BeginPlay()
{
	Super::BeginPlay();

	//当たり判定(ここで判定を入れるとすでに重なっているものもbeginが発生)
	//m_pMesh->SetCollisionProfileName("MyBlockDynamic");

	//ゲームインスタンスに登録
	GetGameInstance<UMyGameInstance>()->SetPlayer(this);
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

//ソケットへの追加
void APlayerChara::AddSocket(AActor* _p, FVector _Pos)
{
	m_pSocket.Add(_p);
	m_pSocketPos.Add(_Pos);
}

//上方向の向きの取得
FRotator APlayerChara::GetUpRotator()
{
	return UKismetMathLibrary::FindLookAtRotation(FVector(0.0f), GetActorUpVector());
}

//影響を受けている重力の数を取得
int APlayerChara::GetGraNum()
{
	return m_pPlanets.Num();
}

//速度の取得
float APlayerChara::GetSpeed()
{
	//return m_Speed;
	return m_pMovement->Velocity.Length();
}

//回転の更新
void APlayerChara::UpdateRotation(float DeltaTime)
{
	//入力がある時には動かせる
	//ないときには勝手に回転する
	if(!m_Rot.IsZero())
	{
		//重心に中心を合わせる
		AddActorLocalOffset(m_RotPivot);

		//現在の向き
		FVector VecDire[3](GetActorForwardVector(), GetActorRightVector(), GetActorUpVector());
		//回転軸
		FVector RotAxis[3](m_pCamera->GetRightVector(), m_pCamera->GetUpVector(), m_pCamera->GetForwardVector());
		//回転後の各ベクトルを作成
		for (int i = 0; i < 3; ++i)
		{
			for (int n = 0; n < 3; ++n)
			{
				VecDire[i] = VecDire[i].RotateAngleAxis(*(&(m_Rot.Pitch) + n) * DeltaTime * m_RotSpeed, RotAxis[n]);
			}
		}
		//回転を適応(3ベクトルからRotatorを生成)
		SetActorRotation(UKismetMathLibrary::MakeRotationFromAxes(VecDire[0], VecDire[1], VecDire[2]));

		//ずらした中心を戻す
		AddActorLocalOffset(-m_RotPivot);

		//次の回転の為にリセット
		m_Rot = FRotator(0.0f, 0.0f, 0.0f);
	}
	else
	{
		//進行方向に徐々に向かせる
		SetActorRotation(UKismetMathLibrary::RInterpTo(GetActorRotation(), m_pMovement->Velocity.Rotation(),DeltaTime, m_ReturnRotSpeed));
	}
}

//移動の更新
void APlayerChara::UpdateMove(float DeltaTime)
{
	//プレイヤーの位置
	FVector Loc(GetActorLocation());
	//進行方向
	FVector MoveDire(0.0f, 0.0f, 0.0f);

	//前進入力のある時
	if (m_ForwardInput != 0.0f)
	{
		//移動方向に前進を加算
		//m_ForwardInputは前後を決める
		MoveDire += m_pMesh->GetForwardVector() * m_ForwardInput * m_ForwardSpeed;

		//次の入力の為にリセット
		m_ForwardInput = 0.0f;
	}

	//重力を受けている時の処理
	if (m_pPlanets.Num() != 0)
	{
		//各重力減に引っ張られる
		for (int i = 0; i < m_pPlanets.Num(); ++i)
		{
			//星に向かう方向
			FVector PlanetDire(m_pPlanets[i]->GetActorLocation() - Loc);
			//星との距離
			float Distance(PlanetDire.Length());
			//向かう強さ(近いほど設定した値に近くなる)
			float Gravity(m_pPlanets[i]->GetGravity() * (1.0f - Distance / m_pPlanets[i]->GetGradius()));

			//進行方向に引力を足す
			MoveDire += PlanetDire.GetSafeNormal() * Gravity;

		}
	}
	//速度の変更
	m_pMovement->Acceleration = MoveDire.Length();
	//移動の反映
	AddMovementInput(MoveDire.GetSafeNormal());

	//ソケットの移動
	//ソケットにモノが入っているならばそれらはまとめて移動
	for (int i = 0; i < m_pSocket.Num(); ++i)
	{
		FVector SPos(m_pSocketPos[i]);
		m_pSocket[i]->SetActorLocation(Loc + GetActorRightVector() * SPos.X + GetActorForwardVector() * -1.0f * SPos.Y + GetActorUpVector() * SPos.Z);
	}

	//移動後の位置
	Loc = GetActorLocation();
	//速度の更新
	m_Speed = (Loc - m_PreLoc).Length() / DeltaTime;
	//前回位置の保存
	m_PreLoc = Loc;

	//重力影響数確認
	UE_LOG(LogTemp, Warning, TEXT("%i"), m_pPlanets.Num());
}

//カメラの更新
void APlayerChara::UpdateCamera(float DeltaTime)
{
	UpdateCameraRot(DeltaTime);
	//UpdateCameraFOV(DeltaTime);
}

//カメラの回転
void APlayerChara::UpdateCameraRot(float DeltaTime)
{
	//カメラに対する入力がある時は回転させる
	//ない時は進行方向を向く角度に戻ろうとする
	if(!m_CameraRotInput.IsZero())
	{
		//操作変更入力がないときは通常の回転
		if(m_ChangeCtrl > 0.0f)
		{
			m_pCamera->AddLocalRotation(m_CameraRotInput * DeltaTime * m_CameraRotSpeed);
			//if (((FVector2D)m_pSpring->GetForwardVector() - FVector2D(0.0f, 0.0f)) <= 1.0f)
			{

			}

			m_ChangeCtrl = 0.0f;
		}
		else
		{
			m_CameraRotInput *= DeltaTime * m_CameraRotSpeed;
			m_CameraRotInput.Roll = 0.0f;
			m_pSpring->AddRelativeRotation(m_CameraRotInput);
		}

		//次の入力に備えてリセット
		m_CameraRotInput = FRotator(0.0f, 0.0f, 0.0f);
	}
	else
	{
		m_pSpring->SetWorldRotation(UKismetMathLibrary::RInterpTo(m_pSpring->GetComponentRotation(),m_pMovement->Velocity.Rotation(), DeltaTime, m_CameraReturnRotSpeed));
		m_pCamera->SetRelativeRotation(UKismetMathLibrary::RInterpTo(m_pCamera->GetRelativeRotation(), FRotator(0.0f, 0.0f, 0.0f), DeltaTime, m_CameraReturnRotSpeed));
	}

	//m_pCamera->SetRelativeRotation(m_CameraRot);
}

//カメラの画角の変更
void APlayerChara::UpdateCameraFOV(float DeltaTime)
{
	m_pCamera->FieldOfView = m_pCamera->FieldOfView + m_pMovement->Velocity.Length() * 0.000001f;

	if (m_pCamera->FieldOfView > 120.0f)
	{
		m_pCamera->FieldOfView = 120.0f;
	}
}

//移動
void APlayerChara::MoveForward(float _value)
{
	if (_value != 0.0f)
	{
		m_ForwardInput = _value;
	}
}

//回転
void APlayerChara::RotPitch(float _value)
{
	if (_value != 0.0f)
	{
		m_Rot.Pitch = _value * 0.5f;
	}
}
void APlayerChara::RotYaw(float _value)
{
	if (_value != 0.0f)
	{
		m_Rot.Yaw = _value * 0.5f;
	}
}
void APlayerChara::RotRoll(float _value)
{
	if (_value != 0.0f)
	{
		m_Rot.Roll = _value * 0.5f;
	}
}

//カメラの回転
void APlayerChara::CameraRotYaw(float _value)
{
	if (_value != 0.0f)
	{
		m_CameraRotInput.Pitch = _value;
	}
}
void APlayerChara::CameraRotPitch(float _value)
{
	if (_value != 0.0f)
	{
		m_CameraRotInput.Yaw = _value;
	}
}

//操作変更
void APlayerChara::ChangeCtrl(float _value)
{
	m_ChangeCtrl = _value;
}

//コリジョンプリセット変更
void APlayerChara::ChangeCollision()
{
	m_pMesh->SetCollisionProfileName("MyBlockDynamic");
}