#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BGMManager.generated.h"

// 前方宣言（includeを減らしてコンパイルを軽くする）
class UAudioComponent;
class USoundBase;

/**
 * BGM を管理するための常駐Actor
 *
 * ・レベルに1つ置くだけで BGM を鳴らせる
 * ・再生 / 停止 / フェード / 曲変更 をまとめて担当する
 *
 * 使い方（例）
 * 1) BP_BGMManager（このクラス派生BP）を作成
 * 2) レベルに1つ配置
 * 3) DefaultBGM を設定し、bAutoPlayDefaultBGM を true にする
 *
 * ※レベル遷移でBGMを継続したい場合は、後から GameInstance 管理にするのが理想
 */
UCLASS()
class SWING_API ABGMManager : public AActor
{
	GENERATED_BODY()

public:
	ABGMManager();

protected:
	// ゲーム開始時に呼ばれる
	virtual void BeginPlay() override;

public:
	/**
	 * BGMを再生する
	 * @param BGM         再生する音源
	 * @param FadeInTime  フェードイン時間（0なら即再生）
	 */
	UFUNCTION(BlueprintCallable, Category = "Audio|BGM")
	void PlayBGM(USoundBase* BGM, float FadeInTime = 0.5f);

	/**
	 * BGMを停止する
	 * @param FadeOutTime フェードアウト時間（0なら即停止）
	 */
	UFUNCTION(BlueprintCallable, Category = "Audio|BGM")
	void StopBGM(float FadeOutTime = 0.5f);

	/**
	 * BGMを切り替える（フェードアウト → フェードイン）
	 * @param NewBGM       切り替える音源
	 * @param FadeOutTime  フェードアウト時間
	 * @param FadeInTime   フェードイン時間
	 */
	UFUNCTION(BlueprintCallable, Category = "Audio|BGM")
	void ChangeBGM(USoundBase* NewBGM, float FadeOutTime = 0.5f, float FadeInTime = 0.5f);

private:
	// 実際にBGMを鳴らすためのコンポーネント
	// BGMは基本的に「距離減衰なし」で鳴らしたいので AudioComponent を使うのが便利
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	UAudioComponent* BGMComp = nullptr;

	// レベルに置くだけで鳴らしたい場合に設定する「デフォルトBGM」
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|BGM", meta = (AllowPrivateAccess = "true"))
	USoundBase* DefaultBGM = nullptr;

	// BeginPlay で DefaultBGM を自動再生するか
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|BGM", meta = (AllowPrivateAccess = "true"))
	bool bAutoPlayDefaultBGM = true;
};