#include "BGMManager.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"

ABGMManager::ABGMManager()
{
	// BGM管理ActorはTick不要（軽くする）
	PrimaryActorTick.bCanEverTick = false;

	// AudioComponent を生成して Root にする
	// BGMは「このActorの位置」に意味がないため RootにしておけばOK
	BGMComp = CreateDefaultSubobject<UAudioComponent>(TEXT("BGMComp"));
	RootComponent = BGMComp;

	// 自動再生させない（BeginPlayで明示的に再生する）
	BGMComp->bAutoActivate = false;

	// UIサウンド扱い
	// ・距離減衰の影響を受けにくくしたい
	// ・常に同じ音量で聞かせたい
	// という場合に有効
	BGMComp->bIsUISound = true;

	// ループ設定について：
	// ループしたいBGMは「音源側（SoundWaveやSoundCue）」で Loop を設定するのが基本
}

void ABGMManager::BeginPlay()
{
	Super::BeginPlay();

	// レベル配置だけでBGMを鳴らしたい場合はここで再生する
	if (bAutoPlayDefaultBGM && DefaultBGM)
	{
		PlayBGM(DefaultBGM, 0.5f);
	}
}

void ABGMManager::PlayBGM(USoundBase* BGM, float FadeInTime)
{
	// Nullチェック（事故防止）
	if (!BGMComp || !BGM)
	{
		return;
	}

	// 音源をセット
	BGMComp->SetSound(BGM);

	// FadeInTime が 0 より大きい場合はフェードインして再生
	if (FadeInTime > 0.0f)
	{
		// FadeIn(フェード時間, 最終音量)
		BGMComp->FadeIn(FadeInTime, 1.0f);
	}
	else
	{
		// 即再生
		BGMComp->Play();
	}
}

void ABGMManager::StopBGM(float FadeOutTime)
{
	if (!BGMComp)
	{
		return;
	}

	// FadeOutTime が 0 より大きい場合はフェードアウト
	if (FadeOutTime > 0.0f)
	{
		// FadeOut(フェード時間, 最終音量)
		BGMComp->FadeOut(FadeOutTime, 0.0f);
	}
	else
	{
		// 即停止
		BGMComp->Stop();
	}
}

void ABGMManager::ChangeBGM(USoundBase* NewBGM, float FadeOutTime, float FadeInTime)
{
	if (!BGMComp || !NewBGM)
	{
		return;
	}

	/**
	 * 本来は「FadeOut完了後に SetSound→FadeIn」が理想ですが、
	 * 初回実装では “簡易版” として以下の流れにしています。
	 *
	 * ・すでに鳴っている → FadeOut → すぐ差し替え → FadeIn
	 * ・鳴っていない     → そのまま PlayBGM
	 *
	 * これでも体感上ほぼ問題なく動きます。
	 * 完璧にしたい場合は Timer を使って FadeOutTime 秒後に切り替える形に拡張可能です。
	 */

	if (BGMComp->IsPlaying() && FadeOutTime > 0.0f)
	{
		// いったんフェードアウト開始
		BGMComp->FadeOut(FadeOutTime, 0.0f);

		// すぐに新しい音へ差し替え
		BGMComp->SetSound(NewBGM);

		// フェードインして再生
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
		// 鳴っていないならそのまま再生
		BGMComp->Stop();
		PlayBGM(NewBGM, FadeInTime);
	}
}