// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "EnemyFSM.h"


// Sets default values
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple'"));
	if (tempMesh.Succeeded())
	{
		// 메시 데이터 할당
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		// 메시 위치/회전 보정
		GetMesh()->SetRelativeLocationAndRotation(FVector(0.f,0.f,-88.f), FRotator(0.f,-90.f,0.f));
	}
	
	// FSM 컴포넌트 등록 - 적에게 상태관리 두뇌를 부착
	fsm = CreateDefaultSubobject<UEnemyFSM>(TEXT("FSM"));
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

