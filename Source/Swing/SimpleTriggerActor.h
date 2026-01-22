#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "SimpleTriggerActor.generated.h"

// 前方宣言（ヘッダの依存を軽くする）
class USoundBase;

/**
 * 汎用トリガーActor
 * ・ゴール
 * ・エフェクト再生
 * ・イベント発火
 * などをBlueprintで実装するための基盤クラス
 *
 * 追加：トリガー発火時に2DSEを鳴らす機能（UI的な確実に聞こえる音）
 */
UCLASS()
class SWING_API ASimpleTriggerActor : public AActor
{
    GENERATED_BODY()

public:
    ASimpleTriggerActor();

protected:
    /** トリガー判定用Box */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trigger")
    UBoxComponent* TriggerBox;

    /** 1回のみ発火するか */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
    bool bTriggerOnce = true;

    /** 既に発火したか */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trigger")
    bool bTriggered = false;

    // -----------------------------
    // 追加：トリガーSE（2D）
    // -----------------------------

    /** トリガー発火時に鳴らすSE（2D）。未設定なら鳴らさない */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Trigger")
    USoundBase* TriggerSE = nullptr;

    /** SE音量（0～） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Trigger", meta = (ClampMin = "0.0"))
    float TriggerSEVolume = 1.0f;

    /** SEピッチ（0～） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio|Trigger", meta = (ClampMin = "0.0"))
    float TriggerSEPitch = 1.0f;

protected:
    /** Overlap開始 */
    UFUNCTION()
    void OnTriggerBeginOverlap(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    /** Overlap終了（拡張用） */
    UFUNCTION()
    void OnTriggerEndOverlap(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex
    );

    /**
     * トリガー発火イベント（BP実装）
     * ・Actor判定
     * ・Tag判定
     * ・演出・遷移
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Trigger")
    void OnTriggered(AActor* OverlappingActor);

    /**
     * トリガー解除イベント（任意）
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Trigger")
    void OnTriggerReleased(AActor* OverlappingActor);
};
