// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyCamera.generated.h"

//前方宣言
class USpringArmComponent;
class UCameraComponent;

UCLASS(Blueprintable)
class SWING_API AMyCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyCamera();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	//スプリングアーム
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Arm, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* m_pSpringArm;

	//カメラ本体
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* m_pCamera;
};
