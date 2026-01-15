#include "SimpleTriggerActor.h"

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
    TriggerBox->SetGenerateOverlapEvents(true);
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

    bTriggered = true;

#if !UE_BUILD_SHIPPING
    UE_LOG(
        LogTemp,
        Log,
        TEXT("Trigger Activated by %s"),
        *OtherActor->GetName()
    );
#endif

    // Blueprint側に処理委譲
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
    UE_LOG(
        LogTemp,
        Log,
        TEXT("Trigger Released by %s"),
        *OtherActor->GetName()
    );
#endif

    OnTriggerReleased(OtherActor);
}
