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
	m_pSpring = CreateDefaultSubobject<USpringArmComponent>("m_pSpring");
	if (m_pSpring)
	{
		//ルート（コリジョン）にアタッチ
		m_pSpring->SetupAttachment(RootComponent);
		//回転は本体に引っ張られない
		m_pSpring->SetUsingAbsoluteRotation(true);
	}

	//カメラの生成
	m_pCamera = CreateDefaultSubobject<UCameraComponent>("m_pCamera");
	if (m_pCamera)
	{
		//ルート（コリジョン）にアタッチ
		m_pCamera->SetupAttachment(m_pSpring);
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
	InputComponent->BindAxis("Letf_Ctrl", this, &APlayerChara::ChangeCtrl);

	InputComponent->BindAction("O_Key", EInputEvent::IE_Pressed, this, &APlayerChara::ChangeCollision);
}

//ティック
void APlayerChara::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateRotation(DeltaTime);
	UpdateCameraRot(DeltaTime);
	UpdateMove(DeltaTime);

	//UE_LOG(LogTemp, Warning, TEXT("%f"), m_pMovement->Acceleration);

	//進行方向に線を表示
	FVector AcLoc(GetActorLocation());
	DrawDebugLine(GetWorld(), AcLoc, AcLoc + m_MoveDire, FColor::Green);
}

//ゲームスタート時
void APlayerChara::BeginPlay()
{
	Super::BeginPlay();

	//当たり判定(ここで判定を入れるとすでに重なっているものもbeginが発生)
	//m_pMainCollision->SetCollisionProfileName("MyBlockDynamic");

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

//回転の更新
void APlayerChara::UpdateRotation(float DeltaTime)
{
	//if (!m_Rot.IsZero())
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
				VecDire[i] = VecDire[i].RotateAngleAxis(*(&(m_Rot.Pitch) + n) * DeltaTime * m_RotSpeedRate, RotAxis[n]);
			}
		}
		//回転を適応(3ベクトルからRotatorを生成)

		FVector MoveDire[3](m_MoveDire.ForwardVector, m_MoveDire.RightVector, m_MoveDire.UpVector);

		for (int i = 0; i < 3; ++i)
		{
			MoveDire[i] = MoveDire[i].RotateAngleAxis(90.0f, RotAxis[0]);
		}

		//SetActorRotation(UKismetMathLibrary::MakeRotationFromAxes(MoveDire[0], MoveDire[1], MoveDire[2]));

		SetActorRotation(m_MoveDire.Rotation());
		AddActorLocalRotation(FRotator(-90.0f, 0.0f, 0.0));

		//SetActorRotation(m_MoveDire.Rotation() + FRotator(0.0f, 90.0f, 0.0f));

		//ずらした中心を戻す
		AddActorLocalOffset(-m_RotPivot);

		//次の回転の為にリセット
		m_Rot = FRotator(0.0f, 0.0f, 0.0f);
	}
}

//カメラの回転
void APlayerChara::UpdateCameraRot(float DeltaTime)
{
	////スプリングアームは進行方向を向かせる
	////カメラには進行方向を向かせる
	m_pSpring->SetWorldRotation(m_MoveDire.Rotation());

	////カメラに対する入力がある時は回転させる
	//if(!m_CameraRotInput.IsZero())
	//{
	//	m_CameraRot += m_CameraRotInput * DeltaTime * m_CameraRotSpeed;
	//	//次の入力に備えてリセット
	//	m_CameraRotInput = FRotator(0.0f, 0.0f, 0.0f);
	//}
	//else
	//{
	////カメラの回転入力がある時は視点を任意で回転
	////ない時は進行方向を向く角度に戻ろうとする
	//	for (int i = 0; i < 2; ++i)
	//	{
	//		//角度が0に近いときは0にする
	//		/*if (fabsf(*(&(m_CameraRot.Pitch) + i)) < 1.0f)
	//		{
	//			*(&(m_CameraRot.Pitch) + i) = 0.0f;
	//		}
	//		else
	//		{
	//			*(&(m_CameraRot.Pitch) + i) += ((*(&m_CameraRot.Pitch + i) > 0.0f) ? -DeltaTime : DeltaTime) * m_CameraReturnRotSpeed;
	//		}*/

	//		if (fabsf(*(&(m_CameraRot.Pitch) + i)) > 1.0)
	//		{
	//			*(&(m_CameraRot.Pitch) + i) += ((*(&m_CameraRot.Pitch + i) > 0.0f) ? -DeltaTime : DeltaTime) * m_CameraReturnRotSpeed;
	//		}
	//		//小さいなら0にする
	//		else
	//		{
	//			*(&(m_CameraRot.Pitch) + i) = 0.0f;
	//		}
	//	}
	//}

	//m_pCamera->SetRelativeRotation(m_CameraRot);

	//m_pSpring->SetWorldRotation(UKismetMathLibrary::RInterpTo(m_pSpring->GetComponentRotation(), m_MoveDire.Rotation(), DeltaTime,1.0f));
}

//入力有効時のカメラの回転
void APlayerChara::RotationCamera(float DeltaTime)
{

}

//移動の更新
void APlayerChara::UpdateMove(float DeltaTime)
{
	//プレイヤーの位置
	FVector Loc(GetActorLocation());

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
		FVector PlanetDire(m_pPlanets[i]->GetActorLocation() - Loc);

		//星との距離
		float Distance(PlanetDire.Length());

		//向かう強さ(近いほど設定した値に近くなる)
		float Gravity(m_pPlanets[i]->GetGravity() * (1.0f - Distance / m_pPlanets[i]->GetGradius()));

		m_MoveDire = m_MoveDire + PlanetDire.GetSafeNormal() * Gravity;

		//UE_LOG(LogTemp,Warning, TEXT("%i"), m_pPlanets.Num());
	}

	AddMovementInput(m_MoveDire,m_MoveDire.Length());

	//ソケットの移動
	for (int i = 0; i < m_pSocket.Num(); ++i)
	{
		FVector SPos(m_pSocketPos[i]);

		m_pSocket[i]->SetActorLocation(Loc + GetActorRightVector() * SPos.X + GetActorForwardVector() * -1.0f * SPos.Y + GetActorUpVector() * SPos.Z);
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
	m_bChangeCtrl = (bool)_value;
}

//コリジョンプリセット変更
void APlayerChara::ChangeCollision()
{
	m_pMainCollision->SetCollisionProfileName("MyBlockDynamic");
}