// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCamera.h"
#include "ThirdPersonCamera.generated.h"

/**
 * 
 */
UCLASS()
class SWING_API AThirdPersonCamera : public AMyCamera
{
	GENERATED_BODY()

public:
	AThirdPersonCamera();

	void Tick(float DeltaTime) override;

protected:
	void BeginPlay() override;

private:
	//ソケットの相対位置
	UPROPERTY(EditAnywhere,Category = Position,meta = (AllowPrivateAccess = "true"))
	FVector m_SocketPos;

	//ソケット登録情報
	bool m_bRegist;
};
