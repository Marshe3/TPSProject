// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TPSPlayerAnim.generated.h"

/**
 * 
 */
UCLASS()
class TPSPROJECT_API UTPSPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	// 매 프레임 자동 호출 - 소유 폰의 상태를 읽어서 변수 갱신(tick)
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	// 캐릭터의 현재 이동 속도(ABP State Machine의 전이 조건에서 사용)
	UPROPERTY(BlueprintReadOnly, Category = PlayerAnim)
	float speed = 0.f;
};
