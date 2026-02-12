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
#include "Components/SplineComponent.h"
#include "MyCalcu.h"
#include "MyCamera.h"
#include "MyWorldSubsystem.h"

//コンストラクタ
APlayerChara::APlayerChara()
	: m_Velocity(0.0f)
	, m_ReturnCourseVelo(0.0f)
	, m_Rot(0.0f)
	, m_CameraRot(0.0f)
	, m_CameraRotInput(0.0f)
	, m_ForwardInput(0.0f)
	, m_PreForwardInput(0.0f)
	, m_PreRotIn(0.0f)
	, m_NowRotSpeed(0.0f)
	, m_ChangeCtrl(0.0f)
	, m_Speed(0.0f)
	, m_ForwardInputTime(0.0f)
	, m_StrongFOVTimer(0.0f)
	, m_ReachMaxRotSpeed(1.0f)
	, m_bCollisiON(false)
	, m_bCamConChange(false)
	, m_bAutoRot(false)
	, m_bReturnCource(false)
{
	m_pMesh = CreateDefaultSubobject<UStaticMeshComponent>("m_pMesh");
	if (m_pMesh)
	{
		//ルートにする
		//コリジョン持ちがルートじゃないと衝突が起こっても止まらない（仮定）
		RootComponent = m_pMesh;
		//オーバーラップイベントの有効化・登録
		//メッシュのコリジョンで判定を行う
		m_pMesh->SetGenerateOverlapEvents(true);
		m_pMesh->OnComponentBeginOverlap.AddDynamic(this, &APlayerChara::OnOverlapBegin);
		m_pMesh->OnComponentEndOverlap.AddDynamic(this, &APlayerChara::OnOverlapEnd);
		//当たり判定
		m_pMesh->SetCollisionProfileName("MyBlockDynamic");
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
		//初期設定部
	}

	// =========================
	// 変更箇所：AudioComponent を C++ で生成（BP登録なし）
	// =========================

	EngineAudioLow = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineAudioLow"));
	if (EngineAudioLow)
	{
		EngineAudioLow->SetupAttachment(RootComponent);
		EngineAudioLow->bAutoActivate = false;
		EngineAudioLow->bIsUISound = false; // 3D扱い
	}

	EngineAudioHigh = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineAudioHigh"));
	if (EngineAudioHigh)
	{
		EngineAudioHigh->SetupAttachment(RootComponent);
		EngineAudioHigh->bAutoActivate = false;
		EngineAudioHigh->bIsUISound = false;
	}

	RotateAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("RotateAudio"));
	if (RotateAudio)
	{
		RotateAudio->SetupAttachment(RootComponent);
		RotateAudio->bAutoActivate = false;
		RotateAudio->bIsUISound = false;
	}
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
	//カメラ操作変更
	InputComponent->BindAction("V_Key", EInputEvent::IE_Pressed, this, &APlayerChara::ChangeCamCon);
	//機体の自動回転の変更
	InputComponent->BindAction("R_Key", EInputEvent::IE_Pressed, this, &APlayerChara::ChangeAutoRot);
}

//ティック
void APlayerChara::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// =========================
	// 重要：音用入力キャッシュ
	// UpdateMove() で m_ForwardInput を 0 に戻す設計でも、
	// ここで「そのフレームの入力」を保持しておけば音が破綻しない。
	// =========================
	m_ThrustInput01ThisFrame = FMath::Clamp(FMath::Abs(m_ForwardInput), 0.0f, 1.0f);

	// 既存更新
	UpdateRotation(DeltaTime);
	UpdateMove(DeltaTime);
	UpdateCamera(DeltaTime);
	UpdateSocket();

	// ★進捗更新（移動後の位置で計算）
    UpdateCourseProgress(DeltaTime);

	// 音更新
	UpdateEngineAudio(DeltaTime);
	UpdateRotateAudio(DeltaTime);

	// =========================
	// 重要：回転入力は「そのフレームだけ」を使う
	// 次フレームで入力が無ければ 0 に戻っていく（スムージングで自然に無音化）
	// =========================
	m_RotateInput01 = 0.0f;

	//各要素のリセット
	ValueReset();
}

//ゲームスタート時
void APlayerChara::BeginPlay()
{
	Super::BeginPlay();

	//ゲームインスタンスに登録
	//他のオブジェクトがインスタンス経由でアドレスを取得できる
	GetGameInstance<UMyGameInstance>()->SetPlayer(this);
	//常に引っ張られる対象のコーススプラインを取得
	m_pSpline = GetGameInstance<UMyGameInstance>()->GetCourseSpline();

	if (m_pSpline)
	{
		m_SplineLen = m_pSpline->GetSplineLength();
		m_CourseS = m_CourseSPrev = m_CourseSBest = m_CourseSDisplay = 0.0f;
		m_bReverse = false;
	}

	// =========================
	// 変更箇所：ループSEの再生開始（C++のみ）
	// =========================
	if (EngineAudioLow && EngineLowLoopSound)
	{
		EngineAudioLow->SetSound(EngineLowLoopSound);
		EngineAudioLow->Play();
	}

	if (EngineAudioHigh && EngineHighLoopSound)
	{
		EngineAudioHigh->SetSound(EngineHighLoopSound);
		EngineAudioHigh->Play();
	}

	if (RotateAudio && RotateLoopSound)
	{
		RotateAudio->SetSound(RotateLoopSound);
		RotateAudio->Play();
	}
}

void APlayerChara::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//相手が重力の場合配列に追加
	if (OtherComp->ComponentHasTag("Gravity")) { m_pPlanets.Add(Cast<APlanet>(OtherActor)); }
	//UE_LOG(LogTemp, Warning, TEXT("%f , %f"), Cast<USphereComponent>(OtherComp)->GetScaledSphereRadius(), (OtherComp->GetComponentLocation() - GetActorLocation()).Length());
}

void APlayerChara::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//相手が重力の場合配列から削除
	if (OtherComp->ComponentHasTag("Gravity")) { m_pPlanets.Remove(Cast<APlanet>(OtherActor)); }
}

//ソケットへの追加
void APlayerChara::AddSocket(AActor* _p, FVector _Pos, bool _bRot)
{
	//pがないと処理しない
	if (_p)
	{
		m_pSocket.Add(_p);
		m_SocketPos.Add(_Pos);
		m_bSocketRot.Add(_bRot);
	}
}
//ソケットからの除外
void APlayerChara::RemoveSocket(AActor* _p)
{
	if (_p)
	{
		//m_pSocket.Get
	}
}

//カメラを無理矢理変更
void APlayerChara::ChangeCamera(USpringArmComponent* _pSpring, UCameraComponent* _pCamera)
{
	if (_pSpring && _pCamera)
	{
		m_pSpring = _pSpring;
		m_pCamera = _pCamera;
	}
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
float APlayerChara::GetMaxSpeed()
{
	return m_pMovement->MaxSpeed;
}

//入力の取得
float APlayerChara::GetForwardInput() const
{
	return m_ForwardInput;
}

//前進入力の継続時間を取得
float APlayerChara::GetForwardInputTime() const
{
	return m_ForwardInputTime;
}

//状態の強制変更（スタート時に使う）
void APlayerChara::SetStart(FVector _Loc, FRotator _Rot, float _Speed)
{
	SetActorRotation(_Rot);
	SetActorLocation(_Loc);
	m_pMovement->Velocity = m_pMesh->GetForwardVector() * _Speed;
}
//減速
void APlayerChara::SubSpeed(float _Rate)
{
	m_pMovement->Velocity *= _Rate;
}

//回転の更新
void APlayerChara::UpdateRotation(float DeltaTime)
{
	//回転入力がある時には動かせる
	if (!m_Rot.IsZero())
	{
		//入力分を足しこみ1以上にはしない
		//m_NowRotSpeed = (m_NowRotSpeed += DeltaTime > 1.0f) ? 1.0f : m_NowRotSpeed += DeltaTime;

		//各方向の入力の有無により増加減少
		for (int i = 0; i < 3; ++i)
		{
			if (*(&m_Rot.Pitch + i)  != 0.0f) 
			{
				//方向が変わったら即0に
				if (FMath::Sign(*(&m_Rot.Pitch + i)) != FMath::Sign(*(&m_PreRotIn.Pitch + i))) { *(&m_NowRotSpeed.Pitch + i) = 0.0f; }

				*(&m_NowRotSpeed.Pitch + i) += FMath::Sign(*(&m_Rot.Pitch + i)) * DeltaTime / m_ReachMaxRotSpeed;
			}
			else
			{
				*(&m_NowRotSpeed.Pitch + i) -= FMath::Sign(*(&m_NowRotSpeed.Pitch + i)) * DeltaTime / m_ReachMaxRotSpeed;
			}
			//0～1に収める
			*(&m_NowRotSpeed.Pitch + i) = FMath::Clamp(fabs(*(&m_NowRotSpeed.Pitch + i)), 0.0f, 1.0f) * FMath::Sign(*(&m_NowRotSpeed.Pitch + i));
		}

		//重心に中心を合わせる
		AddActorLocalOffset(m_RotPivot);

		//現在の向き
		FVector VecDire[3](GetActorForwardVector(), GetActorRightVector(), GetActorUpVector());
		//回転軸
		//FVector RotAxis[3](m_pCamera->GetRightVector(), m_pCamera->GetUpVector(), m_pCamera->GetForwardVector());
		FVector RotAxis[3](m_pSpring->GetRightVector(), m_pSpring->GetUpVector(), m_pSpring->GetForwardVector());
		//回転後の各ベクトルを作成
		for (int i = 0; i < 3; ++i)
		{
			for (int n = 0; n < 3; ++n)
			{

				VecDire[i] = VecDire[i].RotateAngleAxis((*(&m_Rot.Pitch + n)) * fabs(*(&m_NowRotSpeed.Pitch + n)) * DeltaTime * m_MaxRotSpeed, RotAxis[n]);
			}
		}
		//回転を適応(3ベクトルからRotatorを生成)
		SetActorRotation(UKismetMathLibrary::MakeRotationFromAxes(VecDire[0], VecDire[1], VecDire[2]));

		//ずらした中心を戻す
		AddActorLocalOffset(-m_RotPivot);
	}
	//自動回転有効時は勝手に進行方向を向く
	else
	{
		//回転入力が無い時の回転速度の減少
		//m_NowRotSpeed = (m_NowRotSpeed -= DeltaTime < 0.0f) ? 0.0f : m_NowRotSpeed -= DeltaTime;

		for (int i = 0; i < 3; ++i)
		{
			*(&m_NowRotSpeed.Pitch + i) -= FMath::Sign(*(&m_NowRotSpeed.Pitch + i)) * DeltaTime / m_ReachMaxRotSpeed;
		}

		//コース外に出た時
		if (m_ForwardInput == 0.0f || m_bReturnCource)
		{
			//進行方向に徐々に向かせる
			SetActorRotation(UKismetMathLibrary::RInterpTo(GetActorRotation(), m_pMovement->Velocity.Rotation(), DeltaTime, m_ReturnRotSpeed * (1.0f - (m_NowRotSpeed.Pitch + m_NowRotSpeed.Yaw + m_NowRotSpeed.Roll) / 3.0f)));
		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("Input : RotSpeed \n%f,%f\n%f,%f\n%f,%f"), m_Rot.Pitch, m_NowRotSpeed.Pitch, m_Rot.Yaw, m_NowRotSpeed.Yaw, m_Rot.Roll, m_NowRotSpeed.Roll);
	
	//直前の入力を記録
	m_PreRotIn = m_Rot;
}

//移動の更新
void APlayerChara::UpdateMove(float DeltaTime)
{
	FVector Loc(GetActorLocation());		//プレイヤーの位置
	FVector AddMoveDire(0.0f);		//足す進行方向
	FVector AddReturn(0.0f);
	float CameraLagDistance(m_pSpring->CameraLagMaxDistance);	//現在のカメラのラグの最大距離

	//ーーーーーーーーーーーーーーーー
	//移動入力がある時の処理
	//ーーーーーーーーーーーーーーーー
	if (FMath::Abs(m_ForwardInput) != 0.0f)
	{
		//前進と減速の分岐
		if (m_ForwardInput >= 0.0f)
		{
			//移動方向に前進を加算
			//m_ForwardInputは前後を決める
			AddMoveDire += m_pMesh->GetForwardVector() * m_ForwardInput * m_ForwardSpeed;
		}
		else
		{
			AddMoveDire += m_pMovement->Velocity * -1.0f * 0.8f;
		}

		//入力時間の変化
		m_ForwardInputTime += DeltaTime;
	}
	else
	{
		m_ForwardInputTime -= DeltaTime;
	}

	//入力の変化（有効化）があった少し後は画角を広くする
	if (m_PreForwardInput != m_ForwardInput && m_PreForwardInput == 0.0 && m_StrongFOVTimer == 0.0f)
	{
		m_StrongFOVTimer = 0.1f;
	}
	//直前の入力を保存
	m_PreForwardInput = m_ForwardInput;

	//カメラのラグの距離を変化
	m_pSpring->CameraLagMaxDistance = FMath::Clamp(CameraLagDistance, 0.1f, m_CameraLagMaxDistance);

	//前進入力の時間を範囲内に収める
	m_ForwardInputTime = FMath::Clamp(m_ForwardInputTime, 0.0f, 1.0f);
	//UE_LOG(LogTemp, Warning, TEXT("%f"), m_pSpring->CameraLagMaxDistance);

	//ーーーーーーーーーーーーーーーー
	//重力を受けている時の処理
	//ーーーーーーーーーーーーーーーー
	if (m_pPlanets.Num() != 0)
	{
		//重力
		FVector GravityVec(0.0f);
		//一番近い惑星のインデックス
		int NearID(0);

		//各重力減に引っ張られる
		for (int i = 0; i < m_pPlanets.Num(); ++i)
		{
			FVector PlanetDire(m_pPlanets[i]->GetActorLocation() - Loc);	//星に向かう方向
			float Distance(PlanetDire.Length() - m_pPlanets[i]->GetRadius());	//星の表面との距離
			//向かう強さ(近いほど設定した値に近くなる)
			//float Gravity(m_pPlanets[i]->GetGravity() * (1.0f - Distance / (m_pPlanets[i]->GetGradius() - m_pPlanets[i]->GetRadius())));
			float Gravity(m_pPlanets[i]->GetGravity() * (1.0f - Distance / (m_pPlanets[i]->GetGradius() - m_pPlanets[i]->GetRadius())));
			//float Gravity(m_pPlanets[i]->GetGravity() * (10.0f - (Distance / (m_pPlanets[i]->GetGradius() - m_pPlanets[i]->GetRadius())) * 10.0f));
			//float Gravity(m_pPlanets[i]->GetGravity() / (Distance * Distance));

			//UE_LOG(LogTemp, Warning, TEXT("%f"), Gravity);
			DrawDebugLine(GetWorld(), Loc, Loc + PlanetDire, FColor::Red);

			//進行方向に引力を足す
			GravityVec += PlanetDire.GetSafeNormal() * Gravity;
			//一番近い惑星の判定
			//一つ前の星との距離と比較
			if ((m_pPlanets[NearID]->GetActorLocation() - Loc).Length() > Distance)
			{
				NearID = i;
			}

			//
			//確認
			/*UE_LOG(LogTemp, Warning, TEXT("%f * (1.0 - (%f / %f = %f) = %f) = %f"),
				m_pPlanets[i]->GetGravity(),
				Distance, m_pPlanets[i]->GetGradius() - m_pPlanets[i]->GetRadius(),
				Distance / (m_pPlanets[i]->GetGradius() - m_pPlanets[i]->GetRadius()),
				1.0f - Distance / (m_pPlanets[i]->GetGradius() - m_pPlanets[i]->GetRadius()),
				Gravity);*/
		}

		//遠心力を足す
		//MoveDire += PlanetDire.GetSafeNormal() * -1.0f * m_pMovement->Velocity.Length() / Distance;
		//MoveDire += PlanetDire.GetSafeNormal() * -1.0f * (m_pMovement->Velocity.Length() / Distance);
		//CentrifugalVec += (PlanetDire.GetSafeNormal() * -1.0f) * m_pMovement->Velocity.Length() * (1.0f - Distance / m_pPlanets[i]->GetGradius() * 0.000001f);
		//CentrifugalVec += (PlanetDire.GetSafeNormal() * -1.0f) * m_pPlanets[i]->GetGravity() * (1.0f - Distance / m_pPlanets[i]->GetGradius());
		//CentrifugalVec += (PlanetDire.GetSafeNormal() * -1.0f) * UKismetMathLibrary::Dot_VectorVector(m_pMovement->Velocity, PlanetDire.RotateAngleAxis(90.0f,m_pMesh->GetUpVector()));

		////一番近い星の方向
		//FVector NearPlanetDire(m_pPlanets[NearID]->GetActorLocation() - Loc);
		////距離
		//float NearDistance(NearPlanetDire.Length() - m_pPlanets[NearID]->GetRadius());
		//FVector XVec(UKismetMathLibrary::Cross_VectorVector(NearPlanetDire, NowModeDire).GetSafeNormal());
		//FVector YVec(UKismetMathLibrary::Cross_VectorVector(NearPlanetDire, XVec).GetSafeNormal());
		////星との平行な移動量（縦横）
		//float X(UKismetMathLibrary::Dot_VectorVector(XVec, NowModeDire));
		//float Y(UKismetMathLibrary::Dot_VectorVector(YVec, NowModeDire));
		////星に平行な移動量
		//float ParallelMove(sqrtf(X * X + Y * Y));
		////遠心力
		////FVector CentrifugalVec((NearPlanetDire.GetSafeNormal() * -1.0f) * m_ForwardSpeed * (NearDistance / m_pPlanets[NearID]->GetGradius()));
		//FVector CentrifugalVec(0.0f);

		////確認
		////移動方向に足す
		AddMoveDire += GravityVec;

		////デバッグ
		//{
		//	//星に向かう方向
		//	FVector PlanetDire(m_pPlanets[0]->GetActorLocation() - Loc);
		//	//星との距離
		//	float Distance(PlanetDire.Length());
		//	//向かう強さ(近いほど設定した値に近くなる)
		//	float Gravity(m_pPlanets[0]->GetGravity() * (1.0f - Distance / m_pPlanets[0]->GetGradius()));

		//	//UE_LOG(LogTemp, Warning, TEXT("%f"), (m_pPlanets[0]->GetGravity() * (1.0f - (m_pPlanets[0]->GetActorLocation() - Loc).Length() / m_pPlanets[0]->GetGradius())));
		//	//UE_LOG(LogTemp, Warning, TEXT("%f , %f , %f , %f"), Distance,m_pPlanets[0]->GetGradius(), m_pPlanets[0]->GetGravity(), Gravity);
		//
		//	//星平行方向
		//	DrawDebugLine(GetWorld(), Loc, Loc + NearPlanetDire * 50.0f, FColor::White);
		//	DrawDebugLine(GetWorld(), Loc, Loc + XVec * 50.0f, FColor::Cyan);
		//	DrawDebugLine(GetWorld(), Loc, Loc + YVec * 50.0f, FColor::Purple);
		//	//重力と遠心力方向に線を表示
		//	//DrawDebugLine(GetWorld(), Loc, Loc + CentrifugalVec, FColor::Blue);
		//}
	}

	//移動を加算
	//上で変化させた値を足す
	m_Velocity += AddMoveDire * DeltaTime;

	//ーーーーーーーーーーーーーーーー
	//コースに戻る補正
	//ーーーーーーーーーーーーーーーー
	//常時コースの中心に戻ろうとする
	//戻る距離が0の時は未設定扱い
	if (m_pSpline && m_ReturnCourseLen != 0.0f)
	{
		FVector NearCourseLoc(m_pSpline->FindLocationClosestToWorldLocation(Loc, ESplineCoordinateSpace::World));	//スプライン上の最近点
		FVector NearCourseVec(NearCourseLoc - Loc);		//最近点に向かうベクトル
		float NearCourseLen(NearCourseVec.Length());	//最近点との距離

		//コースの中心に向かうベクトル
		//離れているほどに長くなる
		//AddReturn = NearCourseVec.GetSafeNormal()* NearCourseLen * m_ReturnCourseSpeed * DeltaTime;

		//変化の可能性がある前に固定
		m_bReturnCource = false;

		//距離が離れすぎた時は強めに戻す
		if (NearCourseLen >= m_ReturnCourseLen)
		{
			//m_bReturnCource = true;
			//AddReturn = NearCourseVec * m_ReturnCourseSpeed * DeltaTime;
			//m_ReturnCourseVelo += AddReturn;
			//AddReturn -= m_pMovement->Velocity * DeltaTime;

			//進行方向に向かせた後に内側を向かせる
			m_pMovement->Velocity = m_pSpline->FindDirectionClosestToWorldLocation(Loc, ESplineCoordinateSpace::World) * (m_pMovement->Velocity.Length() * M_CourseOutRate);
			m_pMovement->Velocity += NearCourseVec * DeltaTime;

			//コース外であることを示す
			m_bReturnCource = true;

			//現在の移動方向をコース中心方向に回転させる
			//m_pMovement->Velocity = (UKismetMathLibrary::FindLookAtRotation(m_pSpline->FindDirectionClosestToWorldLocation(Loc, ESplineCoordinateSpace::World), NearCourseVec.GetSafeNormal()) * DeltaTime * m_ReturnCourseSpeed).RotateVector(m_pSpline->FindDirectionClosestToWorldLocation(Loc, ESplineCoordinateSpace::World));
			//m_pMovement->Velocity = (UKismetMathLibrary::FindLookAtRotation(m_pMovement->Velocity.GetSafeNormal(), NearCourseVec.GetSafeNormal()) * DeltaTime * m_ReturnCourseSpeed).RotateVector(m_pMovement->Velocity);

			//少し内側を向かうベクトル
			//FVector InSideVec((UKismetMathLibrary::FindLookAtRotation(m_pSpline->FindDirectionClosestToWorldLocation(Loc, ESplineCoordinateSpace::World), NearCourseVec.GetSafeNormal()))
			//FRotator InSideVec((UKismetMathLibrary::FindLookAtRotation(m_pSpline->FindDirectionClosestToWorldLocation(Loc, ESplineCoordinateSpace::World), NearCourseVec.GetSafeNormal())));
			//FVector ISV((InSideVec * m_ReturnCourseSpeed).RotateVector(m_pSpline->FindDirectionClosestToWorldLocation(Loc, ESplineCoordinateSpace::World)));
			//AddMovementInput(ISV.GetSafeNormal());
		}

		/*AddReturn -= m_ReturnCourseVelo;
		m_ReturnCourseVelo = NearCourseVec * NearCourseLen * 0.1f;
		AddReturn += m_ReturnCourseVelo;*/

		//AddReturn = NearCourseVec * NearCourseLen * DeltaTime * DeltaTime;

		//スプラインの位置の確認用
		DrawDebugLine(GetWorld(), Loc, Loc + NearCourseVec, FColor::Orange);
		//UE_LOG(LogTemp, Warning, TEXT("%f"), NearCourseLen);
	}

	//最終的移動変更
	//m_pMovement->Velocity = m_Velocity + AddReturn;
	//m_pMovement->Velocity += AddMoveDire * DeltaTime + AddReturn;
	m_pMovement->Acceleration = (AddMoveDire + AddReturn).Length();
	AddMovementInput((AddMoveDire + AddReturn).GetSafeNormal());

	//移動後の位置
	Loc = GetActorLocation();
	//速度の更新
	m_Speed = (Loc - m_PreLoc).Length() / DeltaTime;
	//前回位置の保存
	m_PreLoc = Loc;

	//FVector NowVelo(m_pMovement->Velocity);
	//UE_LOG(LogTemp, Warning, TEXT("MoveVelo(%f,%f,%f) , AddVelo(%f,%f,%f) , Return(%f,%f,%f)"), NowVelo.X, NowVelo.Y, NowVelo.Z, m_Velocity.X, m_Velocity.Y, m_Velocity.Z,AddReturn.X, AddReturn.Y, AddReturn.Z);

	//方向確認
	//推進方向を表示
	DrawDebugLine(GetWorld(), Loc, Loc + m_pMesh->GetForwardVector() * m_ForwardSpeed, FColor::Yellow);
	//移動方向を線として表示
	DrawDebugLine(GetWorld(), Loc, Loc + m_pMovement->Velocity, FColor::Green);
}



//カメラの更新
void APlayerChara::UpdateCamera(float DeltaTime)
{
	UpdateCameraRot(DeltaTime);
	UpdateCameraMove(DeltaTime);
	UpdateCameraFOV(DeltaTime);
}

//カメラの回転
void APlayerChara::UpdateCameraRot(float DeltaTime)
{
	//カメラに対する入力がある時は回転させる
	//ない時は進行方向を向く角度に戻ろうとする
	if (!m_CameraRotInput.IsZero())
	{
		//操作変更入力がないときは通常の回転
		if (m_ChangeCtrl > 0.0f)
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
		m_pCamera->SetRelativeRotation(UKismetMathLibrary::RInterpTo(m_pCamera->GetRelativeRotation(),m_DefaCameraRot, DeltaTime, m_CameraReturnRotSpeed));
	}

	//コースに戻る時は変化しない
	//else if (!m_bReturnCource)
	{
		FRotator MidRot((m_pMovement->Velocity.GetSafeNormal() + m_pMesh->GetForwardVector()).Rotation());	//進行方向とメッシュ向きの中間方向
		//float VecDot(FVector::DotProduct(m_pMovement->Velocity.GetSafeNormal(), m_pMesh->GetForwardVector()));	//進行方向とメッシュ向きの内積
		//FRotator AddRot((m_pMesh->GetForwardVector().Rotation() - MidRot) * MyCalcu::Clamp((1.0f - (VecDot + 1.0f) / 2.0f),0.0f,1.0f));	//追加する回転

		m_pSpring->SetWorldRotation(UKismetMathLibrary::RInterpTo(m_pSpring->GetComponentRotation(), MidRot, DeltaTime, m_CameraReturnRotSpeed));
		//m_pCamera->SetRelativeRotation(UKismetMathLibrary::RInterpTo(m_pCamera->GetRelativeRotation(), FRotator(-20.0f, 0.0f, 0.0f), DeltaTime, m_CameraReturnRotSpeed));
	}

	//次の入力の為にリセット
	/*m_ForwardInput = 0.0f;*/

	//m_pCamera->SetRelativeRotation(m_CameraRot);
}

//カメラの位置更新
void APlayerChara::UpdateCameraMove(float DeltaTime)
{
	FVector DefaPos(GetActorLocation() + m_DefaAddSpringPos);	//通常の位置
	FVector LagDire(m_pMesh->GetForwardVector() * m_CameraLagMaxDistance);		//カメラのずれる方向
	FVector CamDire[2](m_pSpring->GetRightVector(), m_pSpring->GetUpVector());	//カメラの右と上方向

	FVector2D XY(0.0f);		//ずれる方向をカメラから見て平面方向のみに（奥方向も行うとFOVと混ざる）
	XY.X = (FVector::DotProduct(CamDire[0], LagDire));
	XY.Y = (FVector::DotProduct(CamDire[1], LagDire * 0.618f));	//0.618は1.618を1とした時の1:1.618の1の倍率
	
	//基準の位置に戻る
	if (m_ForwardInput == 0.0f)
	{
		m_pSpring->SetWorldLocation(FMath::VInterpTo(m_pSpring->GetComponentLocation(),DefaPos,DeltaTime, m_CameraLagDistanceSpeed));
	}
	//ずれていく
	else
	{
		m_pSpring->SetWorldLocation(FMath::VInterpTo(m_pSpring->GetComponentLocation(),DefaPos + CamDire[0] * XY.X + CamDire[1] * XY.Y,DeltaTime, m_CameraLagDistanceSpeed));
	}

	//各方向の最大距離
	FVector NowDisVec((m_pSpring->GetComponentLocation() - (GetActorLocation() + m_DefaAddSpringPos)));
	FVector2D NowXYDis(fabs(FVector::DotProduct(CamDire[0], NowDisVec)), fabs(FVector::DotProduct(CamDire[1], NowDisVec)));		//移動後の基準位置からの各方向の距離
	if (NowXYDis[0] > m_CameraLagMaxDistance || NowXYDis[1] > m_CameraLagMaxDistance * 0.618f)
	{
		m_pSpring->SetWorldLocation(DefaPos + CamDire[0] * ((NowXYDis[0] > m_CameraLagMaxDistance) ? XY.X : NowXYDis[0]) + CamDire[1] * ((NowXYDis[1] > m_CameraLagMaxDistance * 0.618f) ? XY.Y : NowXYDis[1]));
	}

	//指定距離より離れそうなときは戻す
	/*if ((m_pSpring->GetComponentLocation() - (GetActorLocation() + m_DefaAddSpringPos)).Length() > m_CameraLagMaxDistance)
	{
		m_pSpring->SetWorldLocation(DefaPos + CamDire[0] * XY.X + CamDire[1] * XY.Y);
	}*/
}

//カメラの画角の変更
void APlayerChara::UpdateCameraFOV(float DeltaTime)
{
	float NowAddFOV(m_pCamera->FieldOfView - m_DefaFOV);	//現在基本値から追加されている画角
	float Rate((m_pMovement->Velocity.Length() / (m_pMovement->MaxSpeed)));	//追加する角度の倍率（0～1）

	//入力直後が否かで変化
	if (m_StrongFOVTimer == 0.0f)
	{
		NowAddFOV = MyCalcu::ToValueF(NowAddFOV, Rate * m_MaxAddFOV, m_DefaAddFOVSpeed, DeltaTime);
	}
	else
	{
		NowAddFOV = MyCalcu::ToValueF(NowAddFOV, Rate * m_MaxAddFOV + m_SpeedUpFOV, m_SpeedUpAddFOVSpeed, DeltaTime);
		m_StrongFOVTimer = MyCalcu::ToValueF(m_StrongFOVTimer, 0.0f, 1.0f, DeltaTime);
	}

	m_pCamera->FieldOfView = m_DefaFOV + MyCalcu::Clamp(NowAddFOV, 0.0f, m_MaxAddFOV + m_SpeedUpFOV);

	//UE_LOG(LogTemp, Warning, TEXT("%ff"), m_StrongFOVTimer);
}

//ソケットの更新
void APlayerChara::UpdateSocket()
{
	//ソケットの移動
	//ソケットにモノが入っているならばそれらはまとめて移動
	for (int i = 0; i < m_pSocket.Num(); ++i)
	{
		FVector SPos(m_SocketPos[i]);
		m_pSocket[i]->SetActorLocation(GetActorLocation() + GetActorRightVector() * SPos.X + GetActorForwardVector() * -1.0f * SPos.Y + GetActorUpVector() * SPos.Z);

		//モノにより回転
		if (m_bSocketRot[i])
		{
			m_pSocket[i]->SetActorRotation(GetActorRotation());
		}
	}
}

//各値のリセット
void APlayerChara::ValueReset()
{
	m_ForwardInput = 0.0f;	//前進入力
	m_Rot = FRotator(0.0f, 0.0f, 0.0f);	//回転入力
}

//移動
void APlayerChara::MoveForward(float _value)
{
	if (_value != 0.0f)
	{
		m_ForwardInput = _value;
	}
}
//減速
void APlayerChara::Deceleration(float _value)
{
	if (_value != 0.0f)
	{

	}
}

//回転
void APlayerChara::RotPitch(float _value)
{
	if (_value != 0.0f)
	{
		m_Rot.Pitch = _value;
		m_RotateInput01 = FMath::Max(m_RotateInput01, FMath::Clamp(FMath::Abs(_value), 0.0f, 1.0f));
	}
}
void APlayerChara::RotYaw(float _value)
{
	if (_value != 0.0f)
	{
		m_Rot.Yaw = _value;
		m_RotateInput01 = FMath::Max(m_RotateInput01, FMath::Clamp(FMath::Abs(_value), 0.0f, 1.0f));
	}
}
void APlayerChara::RotRoll(float _value)
{
	if (_value != 0.0f)
	{
		m_Rot.Roll = _value;
		m_RotateInput01 = FMath::Max(m_RotateInput01, FMath::Clamp(FMath::Abs(_value), 0.0f, 1.0f));
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
	if (!m_bCollisiON)
	{
		m_pMesh->SetCollisionProfileName("MyBlockDynamic");
	}
	else
	{
		m_pMesh->SetCollisionProfileName("AllIgnore");
	}

	m_bCollisiON = !m_bCollisiON;
}
//カメラ操作変更
void APlayerChara::ChangeCamCon()
{
	m_bCamConChange = !m_bCamConChange;
}
//機体の自動回転の変更
void APlayerChara::ChangeAutoRot()
{
	m_bAutoRot = !m_bAutoRot;
}

// =========================
// 変更箇所：2レイヤーエンジン音
// =========================
void APlayerChara::UpdateEngineAudio(float DeltaTime)
{
	if (!EngineAudioLow && !EngineAudioHigh) return;

	// 入力(0-1)を滑らかに
	EngineThrustSmoothed = FMath::FInterpTo(
		EngineThrustSmoothed,
		m_ThrustInput01ThisFrame,
		DeltaTime,
		EngineInterpSpeed
	);

	// ---- Low Layer：下支え（常に少し鳴らす設計が多い）----
	if (EngineAudioLow && EngineAudioLow->IsPlaying())
	{
		const float PitchLow = FMath::Lerp(EngineLowPitchMin, EngineLowPitchMax, EngineThrustSmoothed);
		const float VolLow = FMath::Lerp(EngineLowVolMin, EngineLowVolMax, EngineThrustSmoothed);

		EngineAudioLow->SetPitchMultiplier(PitchLow);
		EngineAudioLow->SetVolumeMultiplier(VolLow);
	}

	// ---- High Layer：踏み込みで出る（カーブを付けると気持ちいい）----
	// 直線だと「すぐ耳につく」ことがあるので、Powで立ち上がりを抑える
	const float HighT = FMath::Pow(EngineThrustSmoothed, 1.5f);

	if (EngineAudioHigh && EngineAudioHigh->IsPlaying())
	{
		const float PitchHigh = FMath::Lerp(EngineHighPitchMin, EngineHighPitchMax, HighT);
		const float VolHigh = FMath::Lerp(EngineHighVolMin, EngineHighVolMax, HighT);

		EngineAudioHigh->SetPitchMultiplier(PitchHigh);
		EngineAudioHigh->SetVolumeMultiplier(VolHigh);
	}
}

// =========================
// 変更箇所：回転音（ループ再生＋パラメータで無音化）
// =========================
void APlayerChara::UpdateRotateAudio(float DeltaTime)
{
	if (!RotateAudio || !RotateAudio->IsPlaying()) return;

	RotateInputSmoothed = FMath::FInterpTo(
		RotateInputSmoothed,
		m_RotateInput01,
		DeltaTime,
		RotateInterpSpeed
	);

	const float Pitch = FMath::Lerp(RotatePitchMin, RotatePitchMax, RotateInputSmoothed);
	const float Vol = FMath::Lerp(RotateVolMin, RotateVolMax, RotateInputSmoothed);

	RotateAudio->SetPitchMultiplier(Pitch);
	RotateAudio->SetVolumeMultiplier(Vol);

	// RotateVolMin=0 の場合：無音でも再生維持するのでクリック/プチノイズが起きにくい
}

void APlayerChara::UpdateCourseProgress(float DeltaTime)
{
	if (!m_pSpline || m_SplineLen <= 0.0f) return;

	const FVector Loc = GetActorLocation();

	// 最近点のキー→距離（0..Len）
	const float Key = m_pSpline->FindInputKeyClosestToWorldLocation(Loc);
	float S = m_pSpline->GetDistanceAlongSplineAtSplineInputKey(Key);
	S = FMath::Clamp(S, 0.0f, m_SplineLen);

	// ---- 3Dコースで別区間に吸われてSが飛ぶのを抑制 ----
	// いきなり大きく飛んだら補間（または無視）する
	if (FMath::Abs(S - m_CourseSPrev) > m_CourseJumpLimit)
	{
		// まずは穏当な補間。飛びが酷いなら「前値固定」に変えるのもアリ
		S = FMath::FInterpTo(m_CourseSPrev, S, DeltaTime, m_CourseUIInterp);
	}

	m_CourseSPrev = S;
	m_CourseS = S;

	// ---- 逆走判定（警告用）----
	const FVector TangentDir =
		m_pSpline->GetDirectionAtDistanceAlongSpline(m_CourseS, ESplineCoordinateSpace::World);

	const FVector Vel = (m_pMovement ? m_pMovement->Velocity : FVector::ZeroVector);
	const float Dot = FVector::DotProduct(Vel.GetSafeNormal(), TangentDir);
	m_bReverse = (Dot < -0.2f);

	// ---- UI表示距離：戻らないゲージにしたいなら最大到達を使う ----
	// 逆走でもマイナス方向に進行しない（あなたの課題に直撃）
	m_CourseSBest = FMath::Max(m_CourseSBest, m_CourseS);

	// 表示はスムージング
	m_CourseSDisplay = FMath::FInterpTo(m_CourseSDisplay, m_CourseSBest, DeltaTime, m_CourseUIInterp);
	m_CourseSDisplay = FMath::Clamp(m_CourseSDisplay, 0.0f, m_SplineLen);
}

float APlayerChara::GetCourseProgress01() const
{
	if (m_SplineLen <= 0.0f) return 0.0f;
	return m_CourseSDisplay / m_SplineLen;  // 0..1
}

float APlayerChara::GetCourseRemainingDistance() const
{
	if (m_SplineLen <= 0.0f) return 0.0f;
	return FMath::Max(0.0f, m_SplineLen - m_CourseSDisplay);
}

bool APlayerChara::IsReverseOnCourse() const
{
	return m_bReverse;
}