// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyPawn.h"

// 変更箇所（Audio）
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"

#include "PlayerChara.generated.h"

// 前方宣言
class UCapsuleComponent;
class USpringArmComponent;
class UCameraComponent;
class APlanet;
class UFloatingPawnMovement;
class USplineComponent;
class AMyCamera;

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
	// オーバーラップ時
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

public:
	UFUNCTION(BlueprintCallable)
	void AddSocket(AActor* _p, FVector _Pos, bool _bRot = false);

	UFUNCTION(BlueprintCallable)
	void RemoveSocket(AActor* _p);

	UFUNCTION(BlueprintCallable)
	void ChangeCamera(USpringArmComponent* _pSpring, UCameraComponent* _pCamera);

	UFUNCTION(BlueprintCallable)
	int GetGraNum();

	UFUNCTION(BlueprintCallable)
	float GetSpeed();
	UFUNCTION(BlueprintCallable)
	float GetMaxSpeed();

	UFUNCTION(BlueprintPure, Category = "Input")
	float GetForwardInput() const;

	UFUNCTION(BlueprintCallable)
	float GetForwardInputTime() const;

	FRotator GetUpRotator();

	UPrimitiveComponent* GetSpline() const;

	//状態の強制変更（スタート時に使う）
	UFUNCTION(BlueprintCallable)
	void SetStart(FVector _Loc,FRotator _Rot,float _Speed);
	void SubSpeed(float _Rate);

	// --- 進捗UI用（Blueprintから取得） ---
	UFUNCTION(BlueprintPure, Category = "Course|UI")
	float GetCourseProgress01() const;

	UFUNCTION(BlueprintPure, Category = "Course|UI")
	float GetCourseRemainingDistance() const;

	UFUNCTION(BlueprintPure, Category = "Course|UI")
	bool IsReverseOnCourse() const;

private:
	// 更新
	void UpdateRotation(float DeltaTime);
	void UpdateMove(float DeltaTime);

	void UpdateCamera(float DeltaTime);
	void UpdateCameraMove(float DeltaTime);
	void UpdateCameraRot(float DeltaTime);
	void UpdateCameraFOV(float DeltaTime);

	void UpdateSocket();

	void ValueReset();

	void UpdateCourseProgress(float DeltaTime);

private:
	// 入力
	void MoveForward(float _value);
	void Deceleration(float _value);

	void RotPitch(float _value);
	void RotYaw(float _value);
	void RotRoll(float _value);

	void CameraRotPitch(float _value);
	void CameraRotYaw(float _value);

	void ChangeCtrl(float _value);
	void ChangeCollision();
	void ChangeCamCon();
	void ChangeAutoRot();

	// =========================
	// 変更箇所：SE（Audio）
	// =========================
	void UpdateEngineAudio(float DeltaTime);
	void UpdateRotateAudio(float DeltaTime);

private:
	// コリジョン
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision, meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* m_pMainCollision;

	// メッシュ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* m_pMesh;


	//=======================
	//カメラ関係
	//=======================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* m_pSpring;		// スプリングアーム

	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	FVector m_DefaAddSpringPos;			//通常のスプリングアームの位置

	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* m_pCamera;		//カメラ

	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	FRotator m_DefaCameraRot;			//通常時のカメラの傾き

	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float m_CameraRotSpeed;				//カメラの回転速度

	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float m_CameraReturnRotSpeed;		//カメラが通常の向きの戻る時の速度

	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float m_CameraLagDistanceSpeed;		//カメラとの距離が変化する速度

	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float m_CameraLagMaxDistance;		//カメラとの最大距離

	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float m_DefaFOV;					//通常の視野角

	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float m_MaxAddFOV;					//速度により追加する視野角の最大値

	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float m_SpeedUpFOV;					//入力直後に追加する視野角

	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float m_DefaAddFOVSpeed;			//通常時の視野角の変化速度

	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float m_SpeedUpAddFOVSpeed;			//入力直後の視野角の変化速度

	//=======================
	//移動関係
	//=======================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Move, meta = (AllowPrivateAccess = "true"))
	UFloatingPawnMovement* m_pMovement;	//移動コンポーネント

	UPROPERTY(EditAnywhere, Category = Move, meta = (AllowPrivateAccess = "true"))
	float m_ForwardSpeed;				//前進速度

	UPROPERTY(EditAnywhere, Category = Move, meta = (AllowPrivateAccess = "true"))
	float m_ReturnCourseSpeed;			//コースに戻る速度

	UPROPERTY(EditAnywhere, Category = Move, meta = (AllowPrivateAccess = "true"))
	float m_MinReturnCourseSpeed;		//コースに戻る最低速度

	UPROPERTY(EditAnywhere, Category = Move, meta = (AllowPrivateAccess = "true"))
	float M_CourseOutRate;				//コースアウト時の減速率

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	float m_ForwardInput;


	//=======================
	//回転関係
	//=======================
	UPROPERTY(EditAnywhere, Category = Rotation, meta = (AllowPrivateAccess = "true"))
	float m_MaxRotSpeed;				//最高回転速度

	UPROPERTY(EditAnywhere, Category = Rotation, meta = (AllowPrivateAccess = "true"))
	float m_ReachMaxRotSpeed;			//最高回転速度に達する速度

	UPROPERTY(EditAnywhere, Category = Rotation, meta = (AllowPrivateAccess = "true"))
	float m_ReturnRotSpeed;				//進行方向に向きを合わせに行く速度

	UPROPERTY(EditAnywhere, Category = Rotation, meta = (AllowPrivateAccess = "true"))
	FVector m_RotPivot;					//回転時の中心

	// スプライン
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Course, meta = (AllowPrivateAccess = "true"))
	USplineComponent* m_pSpline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Course, meta = (AllowPrivateAccess = "true"))
	float m_ReturnCourseLen;

	//=======================
	// 進捗UI（Course Progress） ★ここを追加
	//=======================
	float m_SplineLen = 0.0f;        // 全長
	float m_CourseS = 0.0f;          // 現在s（0..Len）
	float m_CourseSPrev = 0.0f;      // 前フレームs（飛び対策）
	float m_CourseSBest = 0.0f;      // 最大到達s（ゲージ戻さない）
	float m_CourseSDisplay = 0.0f;   // 表示用（平滑化）
	bool  m_bReverse = false;        // 逆走フラグ

	UPROPERTY(EditAnywhere, Category = "Course|UI", meta = (AllowPrivateAccess = "true"))
	float m_CourseJumpLimit = 8000.0f;

	UPROPERTY(EditAnywhere, Category = "Course|UI", meta = (AllowPrivateAccess = "true"))
	float m_CourseUIInterp = 8.0f;

	// =========================
	// 変更箇所：Audio 実体（C++のみで生成）
	// =========================

	// ★2レイヤー：低域エンジン
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	UAudioComponent* EngineAudioLow = nullptr;

	// ★2レイヤー：高域エンジン
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	UAudioComponent* EngineAudioHigh = nullptr;

	// 回転音
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	UAudioComponent* RotateAudio = nullptr;

	// 音源（BP登録しない＝C++のDefaults/Detailsでセット）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	USoundBase* EngineLowLoopSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	USoundBase* EngineHighLoopSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	USoundBase* RotateLoopSound = nullptr;

	// =========================
	// 変更箇所：Audio パラメータ
	// =========================

	// 音用：そのフレームの推力入力(0-1)を保持（UpdateMoveで m_ForwardInput を0にしても崩れない）
	float m_ThrustInput01ThisFrame = 0.0f;

	// 音用：そのフレームの回転入力(0-1)（RotPitch/Yaw/Rollから加算ではなく最大で集約）
	float m_RotateInput01 = 0.0f;

	// ---- Engine Low（低域）----
	UPROPERTY(EditAnywhere, Category = "Audio|EngineLow", meta = (AllowPrivateAccess = "true"))
	float EngineLowPitchMin = 0.9f;

	UPROPERTY(EditAnywhere, Category = "Audio|EngineLow", meta = (AllowPrivateAccess = "true"))
	float EngineLowPitchMax = 1.2f;

	UPROPERTY(EditAnywhere, Category = "Audio|EngineLow", meta = (AllowPrivateAccess = "true"))
	float EngineLowVolMin = 0.15f;

	UPROPERTY(EditAnywhere, Category = "Audio|EngineLow", meta = (AllowPrivateAccess = "true"))
	float EngineLowVolMax = 0.6f;

	// ---- Engine High（高域）----
	UPROPERTY(EditAnywhere, Category = "Audio|EngineHigh", meta = (AllowPrivateAccess = "true"))
	float EngineHighPitchMin = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Audio|EngineHigh", meta = (AllowPrivateAccess = "true"))
	float EngineHighPitchMax = 1.6f;

	UPROPERTY(EditAnywhere, Category = "Audio|EngineHigh", meta = (AllowPrivateAccess = "true"))
	float EngineHighVolMin = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Audio|EngineHigh", meta = (AllowPrivateAccess = "true"))
	float EngineHighVolMax = 0.8f;

	// 変化を滑らかにする
	UPROPERTY(EditAnywhere, Category = "Audio|Engine", meta = (AllowPrivateAccess = "true"))
	float EngineInterpSpeed = 8.0f;

	float EngineThrustSmoothed = 0.0f;

	// ---- Rotate ----
	UPROPERTY(EditAnywhere, Category = "Audio|Rotate", meta = (AllowPrivateAccess = "true"))
	float RotatePitchMin = 0.9f;

	UPROPERTY(EditAnywhere, Category = "Audio|Rotate", meta = (AllowPrivateAccess = "true"))
	float RotatePitchMax = 1.3f;

	UPROPERTY(EditAnywhere, Category = "Audio|Rotate", meta = (AllowPrivateAccess = "true"))
	float RotateVolMin = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Audio|Rotate", meta = (AllowPrivateAccess = "true"))
	float RotateVolMax = 0.8f;

	UPROPERTY(EditAnywhere, Category = "Audio|Rotate", meta = (AllowPrivateAccess = "true"))
	float RotateInterpSpeed = 10.0f;

	float RotateInputSmoothed = 0.0f;

	// =========================
	// ここまで変更箇所
	// =========================

private:
	// 既存メンバ
	TArray<APlanet*> m_pPlanets;

	TArray<AActor*> m_pSocket;
	TArray<FVector> m_SocketPos;
	TArray<bool> m_bSocketRot;

	FVector m_PreLoc;
	FVector m_Velocity;
	FVector m_ReturnCourseVelo;

	FRotator m_Rot;
	FRotator m_PreRotIn;
	FRotator m_NowRotSpeed;
	FRotator m_CameraRot;
	FRotator m_CameraRotInput;

	float m_PreForwardInput;
	float m_Speed;
	float m_ForwardInputTime;
	float m_StrongFOVTimer;		//画角をより広くするタイマー

	float m_ChangeCtrl;

	bool m_bCollisiON;
	bool m_bCamConChange;
	bool m_bAutoRot;
	bool m_bReturnCource;
};
