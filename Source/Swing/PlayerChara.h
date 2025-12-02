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

	//重力を受けている物の数
	UFUNCTION(BlueprintCallable)
	int GetGraNum();

private:
	//回転の更新
	void UpdateRotation(float DeltaTime);
	//移動の更新
	void UpdateMove(float DeltaTime);

private:
//入力
	//移動
	void MoveForward(float _value);
	//回転
	void RotYaw(float _value);
	void RotPitch(float _value);
	void RotRoll(float _value);
	//コリジョンプリセット変更
	void ChangeCollision();

private:
	//コリジョン
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = Collision,meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* m_pMainCollision;
	//メッシュ
	UPROPERTY(EditAnywhere,Category = Mesh,meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* m_pMesh;

	//移動コンポーネント
	UPROPERTY(EditAnywhere,Category = Move,meta = (AllowPrivateAccess = "true"))
	UFloatingPawnMovement* m_pMovement;

	//回転速度の倍率
	UPROPERTY(EditAnywhere,Category = Speed,meta = (AllowPrivateAccess = "true"))
	float m_RotSpeedRate;

	//前進速度の倍率
	UPROPERTY(EditAnywhere,Category = Speed,meta = (AllowPrivateAccess = "true"))
	float m_ForwardSpeed;

	UPROPERTY(EditAnywhere, Category = Socket, meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> m_pSocket;		//付属物の入れ物
	UPROPERTY(EditAnywhere, Category = Socket, meta = (AllowPrivateAccess = "true"))
	TArray<FVector> m_pSocketPos;	//付属物の位置
private:
	TArray<APlanet*> m_pPlanets;	//重力を受けている星たち

	FVector m_MoveDire;		//進行方向
	FRotator m_Rot;			//回転角度
	float m_ForwardInput;	//前進速度
};
