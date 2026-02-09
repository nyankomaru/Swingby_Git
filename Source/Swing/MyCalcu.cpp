// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCalcu.h"

MyCalcu::MyCalcu()
{

}

MyCalcu::~MyCalcu()
{

}

//値から値へ遷移
//引数は、現在値、遷移値、遷移速度、デルタタイム
float MyCalcu::ToValueF(float Current, float Target, float Speed, float DeltaTime)
{
	//UE_LOG(LogTemp, Warning, TEXT("%f,%f,%f,%f"), Current, Target, Speed, DeltaTime);

	//時間の経過がないか、到達点と同じなら今と変わらない
	if (DeltaTime == 0.0f || Current == Target)
	{
		return Current;
	}

	//現在と目的の値の差
	float Def(Target - Current);

	//差がほぼないなら完全一致させる
	if (fabs(Def) <= 0.01f)
	{
		return Target;
	}

	//速度分目的値に近づく(*が1を超えると目的値を超えてしまう)
	//return Current + Sign(Def) * Clamp(Speed * DeltaTime, 0.0f, fabs(Def));
	return Current + Def * Clamp(Speed * DeltaTime, 0.0f, 1.0f);
}

//値の範囲限定
template<class X>
X MyCalcu::Clamp(X Value, X Min, X Max)
{
	return (Value < Min) ? Min : (Value > Max) ? Max : Value;
}

//正負の取得(float)
float MyCalcu::Sign(float Value)
{
	return ((((int)Value & (1 << (sizeof(int) * 8 - 1))) == (1 << (sizeof(int) * 8 - 1))) ? -1.0f : 1.0f);
}