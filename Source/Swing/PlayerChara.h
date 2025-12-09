// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyPawn.h"
#include "PlayerChara.generated.h"

//前方宣言
class UCapsuleComponent;
class USpringArmComponent;
class UCameraComponent;
class APlanet;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;

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
	void AddSocket(AActor* _p, FVector _Pos);

	//方向の取得
	FRotator GetUpRotator();

	//重力を受けている物の数の取得
	UFUNCTION(BlueprintCallable)
	int GetGraNum();

	//速度の取得
	UFUNCTION(BlueprintCallable)
	float GetSpeed();

private:
	//回転の更新
	void UpdateRotation(float DeltaTime);
	//移動の更新
	void UpdateMove(float DeltaTime);

	//カメラの回転
	void UpdateCameraRot(float DeltaTime);
	void RotationCamera(float DeltaTime);

private:
//入力
	//移動
	void MoveForward(float _value);
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

private:
	//シーンコンポーネント(Rootにする)
	UPROPERTY(EditAnywhere,Category = Component,meta = (AllowPrivateAccess = "true"))
	USceneComponent* m_pSceneComp;

	//コリジョン
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = Collision,meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* m_pMainCollision;
	//メッシュ
	UPROPERTY(EditAnywhere,Category = Mesh,meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* m_pMesh;

	//スプリングアーム
	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* m_pSpring;
	//カメラ
	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* m_pCamera;

	//カメラの回転速度
	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float m_CameraRotSpeed;
	//カメラが元の向きに戻る速度
	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float m_CameraReturnRotSpeed;

	//移動コンポーネント
	UPROPERTY(EditAnywhere,Category = Move,meta = (AllowPrivateAccess = "true"))
	UFloatingPawnMovement* m_pMovement;
	//前進速度の倍率
	UPROPERTY(EditAnywhere,Category = Speed,meta = (AllowPrivateAccess = "true"))
	float m_ForwardSpeed;

	//回転速度の倍率
	UPROPERTY(EditAnywhere,Category = Rotation,meta = (AllowPrivateAccess = "true"))
	float m_RotSpeedRate;
	//回転の重心
	UPROPERTY(EditAnywhere, Category = Rotation, meta = (AllowPrivateAccess = "true"))
	FVector m_RotPivot;

	//付属物の入れ物
	UPROPERTY(EditAnywhere, Category = Socket, meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> m_pSocket;
	//付属物の位置
	UPROPERTY(EditAnywhere, Category = Socket, meta = (AllowPrivateAccess = "true"))
	TArray<FVector> m_pSocketPos;

private:
	TArray<APlanet*> m_pPlanets;	//重力を受けている星たち

	FVector m_MoveDire;		//進行方向
	FVector m_PreLoc;		//ひとつ前の位置
	FRotator m_Rot;			//回転角度
	FRotator m_CameraRot;	//カメラの向いている方向
	FRotator m_CameraRotInput;	//カメラの回転方向
	float m_ForwardInput;	//前進速度
	float m_Speed;		//移動速度

	float m_ChangeCtrl;
};
