// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class SWING_API MyCalcu
{
public:
	MyCalcu();
	~MyCalcu();

	static float ToValueF(float Current, float Target, float Speed, float DeltaTime);

	template<class X>
	static X Clamp(X Value, X Min, X Max);

	static float Sign(float Value);
};
