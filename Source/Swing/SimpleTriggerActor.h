#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "SimpleTriggerActor.generated.h"

/**
 * 汎用トリガーActor
 * ・ゴール
 * ・エフェクト再生
 * ・イベント発火
 * などをBlueprintで実装するための基盤クラス
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
