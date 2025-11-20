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

public:
	//オーバーラップ時
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//!オーバーラップ時
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	//方向の取得
	FRotator GetUpRotator();

private:
	//回転の更新
	void UpdateRotation(float DeltaTime);
	//移動の更新
	void UpdateMove(float DeltaTime);

private:
//入力
	//移動
	void MoveForward(float _value);
	void MoveForwardKey(float _value);
	//回転
	void RotYaw(float _value);
	void RotYawMouse(float _value);
	void RotPitch(float _value);
	void RotPitchMouse(float _value);
	void RotRoll(float _value);

private:
	//コリジョン
	UPROPERTY(EditAnywhere,Category = Collision,meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* m_pMainCollision;
	//メッシュ
	UPROPERTY(EditAnywhere,Category = Mesh,meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* m_pMesh;

	//スプリングアーム
	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* m_pSpringArm;
	//カメラ
	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* m_pCamera;

	//回転速度の倍率
	UPROPERTY(EditAnywhere,Category = Speed,meta = (AllowPrivateAccess = "true"))
	float m_RotSpeedRate;

	//前進速度の倍率
	UPROPERTY(EditAnywhere,Category = Speed,meta = (AllowPrivateAccess = "true"))
	float m_ForwardSpeed;

private:
	APlanet* m_pPlanet;	//重力の影響を受ける星

	FRotator m_Rot;	//回転角度

	float m_ForwardInput;	//前進速度

	FVector m_MoveDire;	//進行方向
};
