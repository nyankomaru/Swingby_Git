// ObjectIcon.cpp
#include "ObjectIcon.h"

AObjectIcon::AObjectIcon()
{
	PrimaryActorTick.bCanEverTick = true;

	IconMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("IconMesh"));
	RootComponent = IconMesh;

	// 初期スケールは固定（例）
	IconMesh->SetWorldScale3D(FVector(0.5f));
}

void AObjectIcon::BeginPlay()
{
	Super::BeginPlay();
}

void AObjectIcon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateIconTransform();
}

FVector2D AObjectIcon::GetMinimapPosition() const
{
	// XY位置だけ取り出し
	FVector2D Center2D(MinimapCenter.X, MinimapCenter.Y);
	FVector2D Planet2D(PlanetWorldPosition.X, PlanetWorldPosition.Y);

	FVector2D Relative = (Planet2D - Center2D) * MinimapScale;

	// Z回転（Yaw）だけ適用
	const float Rad = FMath::DegreesToRadians(MinimapYaw);
	const float Cos = FMath::Cos(Rad);
	const float Sin = FMath::Sin(Rad);

	return FVector2D(
		Relative.X * Cos - Relative.Y * Sin,
		Relative.X * Sin + Relative.Y * Cos
	);
}

float AObjectIcon::GetMinimapYaw() const
{
	return MinimapYaw;
}

void AObjectIcon::UpdateIconTransform()
{
	// 2Dミニマップ座標に基づく3D位置を作成（Zは惑星の絶対高さを使う）
	FVector2D MiniPos = GetMinimapPosition();
	FVector NewLocation(MiniPos.X, MiniPos.Y, PlanetWorldPosition.Z);

	SetActorLocation(NewLocation);

	// ミニマップ回転（Yawのみ）
	FRotator NewRotation(0.f, GetMinimapYaw(), 0.f);
	SetActorRotation(NewRotation);

	// サイズは常に一定（必要に応じて更新）
	IconMesh->SetWorldScale3D(FVector(0.5f));
}