// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyActor.generated.h"

UCLASS()
class SWING_API AMyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//コリジョンとメッシュを生成するテンプレート
	template<class Collision,class Mesh>
	void CreateColliAndMesh()
	{
		//コリジョンを生成
		m_pMainCollision = CreateDefaultSubobject<Collision>("m_pMainCollision");
		if (m_pMainCollision)
		{
			//ルートに設定
			RootComponent = m_pMainCollision;
			m_pMainCollision->SetSimulatePhysics(false);


		}

		//メッシュを生成
		m_pMesh = CreateDefaultSubobject<Mesh>("m_pMesh");
		if (m_pMesh)
		{
			//ルート（コリジョン）にアタッチ
			m_pMesh->SetupAttachment(RootComponent);
			//メッシュは当たり判定無し
			m_pMesh->SetCollisionProfileName("AllIgnore");
		}
	}

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	//オーバーラップ時のイベント
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtheActor, UPrimitiveComponent* OtherComp,int32 OyherBodyIndex);

protected:
	//コリジョン
	UPROPERTY(EditAnywhere,Category = Collision, meta = (AllowPrivateAccess = "true"))
	UPrimitiveComponent* m_pMainCollision;

	//メッシュ
	UPROPERTY(EditAnywhere, Category = Collision, meta = (AllowPrivateAccess = "true"))
	UMeshComponent* m_pMesh;
};
