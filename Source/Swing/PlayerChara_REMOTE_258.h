// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyPawn.h"

//変更箇所
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"

#include "PlayerChara.generated.h"

//前方宣言
class UCapsuleComponent;
class USpringArmComponent;
class UCameraComponent;
class APlanet;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;
class USplineComponent;

/**
 *
 */
UCLASS()
class SWING_API APlayerChara : public AMyPawn
{
	GENERATED_BODY()

public:
	APlayerChara();

	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Tick(float DeltaTime) override;

protected:
	void BeginPlay() override;

public:
	//オーバーラップ時
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//!オーバーラップ時
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	//ソケットへの追加
	//設定しない限り回転はしない
	UFUNCTION(BlueprintCallable)
	void AddSocket(AActor* _p, FVector _Pos, bool _bRot = false);
	//ソケットからの除外
	UFUNCTION(BlueprintCallable)
	void RemoveSocket(AActor* _p);

	//重力を受けている物の数の取得
	UFUNCTION(BlueprintCallable)
	int GetGraNum();
	//速度の取得
	UFUNCTION(BlueprintCallable)
	float GetSpeed();

	//プレイヤーの入力を取得
	UFUNCTION(BlueprintPure, Category = "Input")
	float GetForwardInput() const;

	//前進入力の継続時間を取得
	UFUNCTION(BlueprintCallable)
	float GetForwardInputTime() const;

	//方向の取得
	FRotator GetUpRotator();

	//スプラインの取得・設定
	UPrimitiveComponent* GetSpline() const;
	void SetSpline(UPrimitiveComponent* _Spline);

	//減速
	void SubSpeed(float _Rate);
private:
	//回転の更新
	void UpdateRotation(float DeltaTime);
	//移動の更新
	void UpdateMove(float DeltaTime);

	//カメラの更新
	void UpdateCamera(float DeltaTime);
	//カメラの位置更新
	void UpdateCameraMove(float DeltaTime);
	//カメラの回転
	void UpdateCameraRot(float DeltaTime);
	//カメラの画角の変更
	void UpdateCameraFOV(float DeltaTime);

	//ソケットの更新
	void UpdateSocket();

private:
	//入力
		//移動
	void MoveForward(float _value);
	//減速
	void Deceleration(float _value);
	//回転
	void RotPitch(float _value);
	void RotYaw(float _value);
	void RotRoll(float _value);

	//カメラの回転
	void CameraRotPitch(float _value);
	void CameraRotYaw(float _value);

	//操作変更
	void ChangeCtrl(float _value);

	//コリジョンプリセット変更
	void ChangeCollision();
	//カメラ操作変更
	void ChangeCamCon();
	//機体の自動回転の変更
	void ChangeAutoRot();

	//変更箇所
	//エンジン音
	void UpdateEngineAudio(float DeltaTime);

private:

	//コリジョン
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision, meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* m_pMainCollision;
	//メッシュ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* m_pMesh;

	//スプリングアーム
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* m_pSpring;
	//カメラ
	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* m_pCamera;
	//カメラの標準の傾き
	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	FRotator m_DefaCameraRot;

	//カメラの回転速度
	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float m_CameraRotSpeed;
	//カメラが元の向きに戻る速度
	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float m_CameraReturnRotSpeed;
	//カメラのラグの距離の変化速度
	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float m_CameraLagDistanceSpeed;
	//カメラのラグの最大距離
	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float m_CameraLagMaxDistance;

	//移動コンポーネント
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Move, meta = (AllowPrivateAccess = "true"))
	UFloatingPawnMovement* m_pMovement;
	//前進速度の倍率
	UPROPERTY(EditAnywhere, Category = Speed, meta = (AllowPrivateAccess = "true"))
	float m_ForwardSpeed;
	//コースに戻る速度
	UPROPERTY(EditAnywhere, Category = Speed, meta = (AllowPrivateAccess = "true"))
	float m_ReturnCourseSpeed;
	//コースに戻る速度の最低値
	UPROPERTY(EditAnywhere, Category = Speed, meta = (AllowPrivateAccess = "true"))
	float m_MinReturnCourseSpeed;
	//コース端の減速率
	UPROPERTY(EditAnywhere, Category = Speed, meta = (AllowPrivateAccess = "true"))
	float M_CourseOutRate;


	//最高回転速度
	UPROPERTY(EditAnywhere, Category = Rotation, meta = (AllowPrivateAccess = "true"))
	float m_MaxRotSpeed;
	//最高回転速度に達する速度
	UPROPERTY(EditAnywhere, Category = Rotation, meta = (AllowPrivateAccess = "true"))
	float m_ReachMaxRotSpeed;
	//進行方向に戻る回転速度
	UPROPERTY(EditAnywhere, Category = Rotation, meta = (AllowPrivateAccess = "true"))
	float m_ReturnRotSpeed;
	//回転の重心
	UPROPERTY(EditAnywhere, Category = Rotation, meta = (AllowPrivateAccess = "true"))
	FVector m_RotPivot;

	//レベル上のコースのスプライン
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Course, meta = (AllowPrivateAccess = "true"))
	USplineComponent* m_pSpline;
	//強い補正を発生させる距離
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Course, meta = (AllowPrivateAccess = "true"))
	float m_ReturnCourseLen;


	//変更箇所
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	UAudioComponent* EngineAudio = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	USoundBase* EngineLoopSound = nullptr;

	//推力(0-1) -> ピッチ/音量
	UPROPERTY(EditAnywhere, Category = "Audio|Engine", meta = (AllowPrivateAccess = "true"))
	float EnginePitchMin = 0.9f;

	UPROPERTY(EditAnywhere, Category = "Audio|Engine", meta = (AllowPrivateAccess = "true"))
	float EnginePitchMax = 1.6f;

	UPROPERTY(EditAnywhere, Category = "Audio|Engine", meta = (AllowPrivateAccess = "true"))
	float EngineVolMin = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Audio|Engine", meta = (AllowPrivateAccess = "true"))
	float EngineVolMax = 1.0f;

	//変化を滑らかにする
	UPROPERTY(EditAnywhere, Category = "Audio|Engine", meta = (AllowPrivateAccess = "true"))
	float EngineInterpSpeed = 8.0f;

	float EngineThrustSmoothed = 0.0f;


	// 回転音の更新（毎フレーム）
	void UpdateRotateAudio(float DeltaTime);

	// 回転入力の「強さ」だけを音用に保持する（0～1）
	// ※m_Rot は UpdateRotation でゼロに戻るので、音用に別管理するのが安全
	float m_RotateInput01 = 0.0f;

	// 回転音を鳴らすコンポーネント（ループ）
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	UAudioComponent* RotateAudio = nullptr;

	// 回転ループ音源（BPで設定）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	USoundBase* RotateLoopSound = nullptr;

	// 0～1（回転入力）→ ピッチ/音量
	UPROPERTY(EditAnywhere, Category = "Audio|Rotate", meta = (AllowPrivateAccess = "true"))
	float RotatePitchMin = 0.9f;

	UPROPERTY(EditAnywhere, Category = "Audio|Rotate", meta = (AllowPrivateAccess = "true"))
	float RotatePitchMax = 1.3f;

	UPROPERTY(EditAnywhere, Category = "Audio|Rotate", meta = (AllowPrivateAccess = "true"))
	float RotateVolMin = 0.0f;   // 回してない時は無音にしたいなら0

	UPROPERTY(EditAnywhere, Category = "Audio|Rotate", meta = (AllowPrivateAccess = "true"))
	float RotateVolMax = 0.8f;

	// 変化を滑らかにする（カクつき防止）
	UPROPERTY(EditAnywhere, Category = "Audio|Rotate", meta = (AllowPrivateAccess = "true"))
	float RotateInterpSpeed = 10.0f;

	float RotateInputSmoothed = 0.0f;

	//ここまで変更箇所

private:
	TArray<APlanet*> m_pPlanets;	//重力を受けている星たち

	TArray<AActor*> m_pSocket;	//付属物の入れ物
	TArray<FVector> m_SocketPos;	//付属物の位置
	TArray<bool> m_bSocketRot;	//付属物が追従回転するかどうかを示す	

	FVector m_PreLoc;		//ひとつ前の位置
	FVector m_Velocity;		//補正を除いた移動量
	FVector m_ReturnCourseVelo;	//ステージに戻るベクトル

	FRotator m_Rot;			//回転入力
	FRotator m_PreRotIn;	//直前の回転入力
	FRotator m_NowRotSpeed;	//現在の回転速度
	FRotator m_CameraRot;	//カメラの向いている方向
	FRotator m_CameraRotInput;	//カメラの回転方向
	float m_ForwardInput;	//前進速度
	float m_PreForwardInput;	//直前の前進入力
	float m_Speed;		//移動速度
	float m_ForwardInputTime;	//前進入力継続時間
	float m_StrongFOVTimer;		//画角をより広くするタイマー

	float m_ChangeCtrl;

	bool m_bCollisiON;	//コリジョン有効の有無
	bool m_bCamConChange;	//カメラ操作変更
	bool m_bAutoRot;	//機体の自動回転の有無
	bool m_bReturnCource;	//コースに
};
