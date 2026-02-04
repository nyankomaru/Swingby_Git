#include "BGMManager.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"

ABGMManager::ABGMManager()
{
	// =========================
	// 最適化：Tick無効化
	// =========================
	// BGM 管理は毎フレーム処理が不要なので Tick を止めて負荷を抑えます。
	PrimaryActorTick.bCanEverTick = false;

	// =========================
	// 構成：AudioComponent作成
	// =========================
	// BGM は「Actor の位置」に意味を持たないため、RootComponent にしてシンプルに管理します。
	BGMComp = CreateDefaultSubobject<UAudioComponent>(TEXT("BGMComp"));
	RootComponent = BGMComp;

	// =========================
	// 再生制御：自動再生OFF
	// =========================
	// 生成直後に勝手に鳴らさず、BeginPlay または外部APIで明示的に再生開始します。
	BGMComp->bAutoActivate = false;

	// =========================
	// 再生特性：UIサウンド扱い
	// =========================
	// 目的：
	// ・距離減衰などの影響を受けにくくしたい
	// ・常に同じ音量感で聞かせたい
	// といった「BGM的な鳴らし方」に寄せるための設定です。
	BGMComp->bIsUISound = true;

	// =========================
	// ループについて
	// =========================
	// ループさせたい BGM は、基本的に音源（SoundWave / SoundCue）側で Loop 設定を行います。
}

void ABGMManager::BeginPlay()
{
	Super::BeginPlay();

	// =========================
	// 起動時BGM：自動再生
	// =========================
	// レベル配置のみで BGM を鳴らす運用の場合は、ここで DefaultBGM を再生します。
	if (bAutoPlayDefaultBGM && DefaultBGM)
	{
		PlayBGM(DefaultBGM, 0.5f);
	}
}

void ABGMManager::PlayBGM(USoundBase* BGM, float FadeInTime)
{
	// =========================
	// 安全策：Nullチェック
	// =========================
	// 設定ミスや未初期化によるクラッシュを防ぎます。
	if (!BGMComp || !BGM)
	{
		return;
	}

	// =========================
	// 音源設定
	// =========================
	// 再生対象の音源を AudioComponent に紐づけます。
	BGMComp->SetSound(BGM);

	// =========================
	// 再生：フェードイン or 即時
	// =========================
	// FadeInTime > 0 ならフェードインしながら開始、0 なら即時開始。
	if (FadeInTime > 0.0f)
	{
		// FadeIn(フェード時間, 最終音量)
		BGMComp->FadeIn(FadeInTime, 1.0f);
	}
	else
	{
		BGMComp->Play();
	}
}

void ABGMManager::StopBGM(float FadeOutTime)
{
	// =========================
	// 安全策：Nullチェック
	// =========================
	if (!BGMComp)
	{
		return;
	}

	// =========================
	// 停止：フェードアウト or 即時
	// =========================
	// FadeOutTime > 0 ならフェードアウトしながら停止、0 なら即時停止。
	if (FadeOutTime > 0.0f)
	{
		// FadeOut(フェード時間, 最終音量)
		BGMComp->FadeOut(FadeOutTime, 0.0f);
	}
	else
	{
		BGMComp->Stop();
	}
}

void ABGMManager::ChangeBGM(USoundBase* NewBGM, float FadeOutTime, float FadeInTime)
{
	// =========================
	// 安全策：Nullチェック
	// =========================
	if (!BGMComp || !NewBGM)
	{
		return;
	}

	// =========================
	// 切替方針：簡易版（初期実装）
	// =========================
	// 理想：
	// ・FadeOut 完了後に SetSound → FadeIn（曲頭が綺麗に出る）
	//
	// ただし初期実装では、体感上の違和感が出にくい範囲で手順を簡略化します。
	//
	// ・再生中：FadeOut 開始 → すぐ差し替え → FadeIn（または即再生）
	// ・停止中：そのまま PlayBGM 相当で再生開始
	//
	// 仕上げを重視する場合：
	// ・Timer を使い「FadeOutTime 秒後に SetSound→FadeIn」へ拡張できます。

	if (BGMComp->IsPlaying() && FadeOutTime > 0.0f)
	{
		// =========================
		// 再生中：フェードアウト開始
		// =========================
		BGMComp->FadeOut(FadeOutTime, 0.0f);

		// =========================
		// 曲差し替え（簡易版のため即時）
		// =========================
		BGMComp->SetSound(NewBGM);

		// =========================
		// 新曲再生：フェードイン or 即時
		// =========================
		if (FadeInTime > 0.0f)
		{
			BGMComp->FadeIn(FadeInTime, 1.0f);
		}
		else
		{
			BGMComp->Play();
		}
	}
	else
	{
		// =========================
		// 停止中：通常再生
		// =========================
		// 念のため停止状態を明確にしてから再生開始します。
		BGMComp->Stop();
		PlayBGM(NewBGM, FadeInTime);
	}
}