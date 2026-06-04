// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h"

// 적이 가질 수 있는 상태 정의
UENUM(BlueprintType) // 에디터랑 BP에서도 사용 가능하도록 설정
enum class EEnemyState : uint8
{
	Idle,
	Move,
	Attack,
	Damage,
	Die
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TPSPROJECT_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEnemyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	// 현재 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FSM)
	EEnemyState mState = EEnemyState::Idle;
	
	// 상태에 따른 동작
	void IdleState();
	void MoveState();
	void AttackState();
	void DamageState();
	void DieState();
	
	// 대기 시간
	UPROPERTY(EditDefaultsOnly, Category = FSM)
	float idleDelayTime = 2.f;
	
	// 경과 시간(누적)
	float currentTime = 0.f;
	
	// 목표 타겟
	UPROPERTY(VisibleAnywhere, Category = FSM)
	class ATPSPlayer* target;
	
	// 소유 액터
	UPROPERTY()
	class AEnemy* me;
	
	// 공격 범위
	UPROPERTY(EditDefaultsOnly, Category = FSM)
	float attackRange = 150.f;
	
	// 공격 대기 시간
	UPROPERTY(EditDefaultsOnly, Category = FSM)
	float attackDelayTime = 1.1f;
	
	// 피격 알림 이벤트 함수
	void OnDamageProcess();
	
	//체력
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,Category = FSM)
	int32 hp = 3;
	
	// 피격 대기시간 (경직)
	UPROPERTY(EditDefaultsOnly, Category = FSM)float damageDelayTime= 1.f;
};
