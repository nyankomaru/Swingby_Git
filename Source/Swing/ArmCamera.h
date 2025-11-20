// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCameraActor.h"
#include "ArmCamera.generated.h"

//前方宣言
class USpringArmComponent;
class APlayerChara;

/**
 * 
 */
UCLASS()
class SWING_API AArmCamera : public AMyCameraActor
{
	GENERATED_BODY()

public:
	AArmCamera();

	void Tick(float DeltaTime) override;

private:
	//スプリングアームの先の設定
	UFUNCTION(BlueprintCallable)
	void SetPoint(AActor* _pActor);
	
private:
	//スプリングアーム
	UPROPERTY(EditAnywhere, Category = Arm, meta = (AllowPrivateAccess = "true"));
	USpringArmComponent* m_pSpringArm;

	//カメラで追う対象
	UPROPERTY(EditAnywhere, Category = Arm, meta = (AllowPrivateAccess = "true"));
	APlayerChara* m_pPoint;

	FVector m_ArmRoot;	//スプリングアームの原点
};
