// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameInstanceSubsystem.h"

void UMyGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}
void UMyGameInstanceSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UMyGameInstanceSubsystem::Tick(float DeltaTime)
{

}
TStatId UMyGameInstanceSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UMyTickableWorldSubsystem, STATGROUP_Tickables);
}