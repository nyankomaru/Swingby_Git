// ObjectIcon.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "ObjectIcon.generated.h"

UCLASS()
class SWING_API AObjectIcon : public AActor
{
	GENERATED_BODY()

public:
	AObjectIcon();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// ミニマップ中心（プレイヤーの位置など）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	FVector MinimapCenter;

	// ミニマップの回転(Yawのみ)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	float MinimapYaw;

	// ミニマップスケール（倍率）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	float MinimapScale = 1.0f;

	// 表示対象の惑星ワールド位置（Zは絶対高さ）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	FVector PlanetWorldPosition;

	// アイコンメッシュ表示用
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minimap")
	UStaticMeshComponent* IconMesh;

	// ミニマップ上の2D座標を計算
	UFUNCTION(BlueprintCallable, Category = "Minimap")
	FVector2D GetMinimapPosition() const;

	// ミニマップ上の回転（Yawのみ）
	UFUNCTION(BlueprintCallable, Category = "Minimap")
	float GetMinimapYaw() const;

private:
	void UpdateIconTransform();
};