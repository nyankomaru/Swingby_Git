// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyActor.h"
#include "Planet.generated.h"

//前方宣言
class USphereComponent;

/**
 * 
 */
UCLASS()
class SWING_API APlanet : public AMyActor
{
	GENERATED_BODY()

public:
	APlanet();

protected:
	virtual void BeginPlay() override;

public:
	//重力の取得
	float GetGravity() const;
	//重力半径の取得
	float GetGradius() const;
	
protected:
	//重力の範囲
	UPROPERTY(EditAnywhere,Category = Gravity,meta = (AllowPrivateAccess = "true"))
	USphereComponent* m_pGravitySphere;
	//重力
	UPROPERTY(EditAnywhere, Category = Gravity, meta = (AllowPrivateAccess = "true"))
	float m_Gravity;
};
