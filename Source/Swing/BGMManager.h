#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BGMManager.generated.h"

// =========================
// 前方宣言：依存を減らす
// =========================
// ヘッダの include を最小限にし、コンパイル時間と影響範囲を抑えるため、型だけ宣言します。
// 実体の include は .cpp 側で行います。
class UAudioComponent;
class USoundBase;

/**
 * =========================
 * BGM管理Actor（常駐）
 * =========================
 *
 * 役割：
 * ・レベル（マップ）内に 1 つ置くだけで BGM 再生を開始できる
 * ・「再生 / 停止 / フェード / 曲切替」の操作をこの Actor に集約する
 *
 * 想定運用（例）：
 * 1) 本クラスを継承した Blueprint（例：BP_BGMManager）を作成
 * 2) レベルに 1 つだけ配置
 * 3) DefaultBGM を設定し、bAutoPlayDefaultBGM を true にする
 *
 * 補足：
 * ・レベル遷移後も BGM を継続する設計にする場合は、GameInstance 管理へ移行するのが一般的です。
 */
UCLASS()
class SWING_API ABGMManager : public AActor
{
	GENERATED_BODY()

public:
	ABGMManager();

protected:
	// =========================
	// 起動時処理
	// =========================
	// Actor の開始タイミングで 1 回だけ呼ばれます。
	virtual void BeginPlay() override;

public:
	// =========================
	// 公開API：BGM操作
	// =========================

	/**
	 * BGMを再生します（必要に応じてフェードイン）。
	 * @param BGM        再生する音源
	 * @param FadeInTime フェードイン時間（秒）。0 なら即時再生
	 */
	UFUNCTION(BlueprintCallable, Category = "Audio|BGM")
	void PlayBGM(USoundBase* BGM, float FadeInTime = 0.5f);

	/**
	 * BGMを停止します（必要に応じてフェードアウト）。
	 * @param FadeOutTime フェードアウト時間（秒）。0 なら即時停止
	 */
	UFUNCTION(BlueprintCallable, Category = "Audio|BGM")
	void StopBGM(float FadeOutTime = 0.5f);

	/**
	 * BGMを切り替えます（フェードアウト → フェードイン）。
	 * @param NewBGM      切り替え先の音源
	 * @param FadeOutTime フェードアウト時間（秒）
	 * @param FadeInTime  フェードイン時間（秒）
	 */
	UFUNCTION(BlueprintCallable, Category = "Audio|BGM")
	void ChangeBGM(USoundBase* NewBGM, float FadeOutTime = 0.5f, float FadeInTime = 0.5f);

private:
	// =========================
	// 内部：再生コンポーネント
	// =========================
	// BGM 実再生用の AudioComponent。
	// BGM は距離減衰を持たせないことが多く、位置依存の処理を避けて一元管理しやすい構成です。
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	UAudioComponent* BGMComp = nullptr;

	// =========================
	// 設定：デフォルトBGM
	// =========================
	// 「レベルに置いただけで鳴らしたい」場合に指定する起動時BGM。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|BGM", meta = (AllowPrivateAccess = "true"))
	USoundBase* DefaultBGM = nullptr;

	// BeginPlay のタイミングで DefaultBGM を自動再生するかどうか。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|BGM", meta = (AllowPrivateAccess = "true"))
	bool bAutoPlayDefaultBGM = true;
};