// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCalcu.h"

MyCalcu::MyCalcu()
{

}

MyCalcu::~MyCalcu()
{

}

float MyCalcu::ToValue(float Current, float Target, float Speed, float DeltaTime)
{
	float ReValue(Target);

	//時間の経過がないか、到達点と同じ
	if (DeltaTime == 0.0f || Current == Target)
	{
		return Current;
	}



	return ReValue;
}