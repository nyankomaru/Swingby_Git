#include "SimpleTriggerActor.h"

#include "Kismet/GameplayStatics.h"   // PlaySound2D
#include "Sound/SoundBase.h"          // USoundBase

ASimpleTriggerActor::ASimpleTriggerActor()
{
    PrimaryActorTick.bCanEverTick = false;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    RootComponent = TriggerBox;

    // デフォルトサイズ
    TriggerBox->SetBoxExtent(FVector(200.f, 200.f, 100.f));

    // Overlap専用
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Overlap);
    TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
    TriggerBox->SetGenerateOverlapEvents(true);

    // 非表示（必要ならBPで変更）
    TriggerBox->SetHiddenInGame(true);

    // イベントバインド
    TriggerBox->OnComponentBeginOverlap.AddDynamic(
        this, &ASimpleTriggerActor::OnTriggerBeginOverlap
    );

    TriggerBox->OnComponentEndOverlap.AddDynamic(
        this, &ASimpleTriggerActor::OnTriggerEndOverlap
    );
}

void ASimpleTriggerActor::OnTriggerBeginOverlap(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    if (!OtherActor || OtherActor == this)
    {
        return;
    }

    // 多重発火防止
    if (bTriggerOnce && bTriggered)
    {
        return;
    }

    // ここで「発火済み」を確定（これ以降は1回きり）
    bTriggered = true;

#if !UE_BUILD_SHIPPING
    UE_LOG(LogTemp, Log, TEXT("Trigger Activated by %s"), *OtherActor->GetName());
#endif

    // -----------------------------
    // 追加：トリガーSE（2D）を鳴らす
    // ・UI的に確実に聞こえる
    // ・距離減衰なし
    // -----------------------------
    if (TriggerSE)
    {
        UGameplayStatics::PlaySound2D(
            this,
            TriggerSE,
            TriggerSEVolume,
            TriggerSEPitch
        );
    }

    // Blueprint側に処理委譲（演出、フェード、レベル遷移など）
    OnTriggered(OtherActor);
}

void ASimpleTriggerActor::OnTriggerEndOverlap(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex
)
{
    if (!OtherActor || OtherActor == this)
    {
        return;
    }

#if !UE_BUILD_SHIPPING
    UE_LOG(LogTemp, Log, TEXT("Trigger Released by %s"), *OtherActor->GetName());
#endif

    OnTriggerReleased(OtherActor);
}
