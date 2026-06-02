// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"


// Sets default values
ABullet::ABullet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// 충돌체 등록
	collisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Component"));
	// 충돌 프로필 설정(모든 물체에 튕김)
	collisionComp->SetCollisionProfileName(TEXT("BlockAll"));
	collisionComp->SetSphereRadius(13.f);
	// 루트컴포넌트로 등록
	RootComponent = collisionComp;
	
	// 외관 컴포넌트 등록
	bodyMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body Mesh Component"));
	// 부모 컴포넌트에 자식으로 등록(충돌체가 움직일때 외관도 움직이게 하기 위함)
	bodyMeshComp->SetupAttachment(collisionComp);
	// 외관 컴포넌트의 충돌은 끔
	bodyMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//외관 크기 설정
	bodyMeshComp->SetRelativeScale3D(FVector(0.25f));
	
	// 발사체 컴포넌트 등록
	movementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	// movement 컴포넌트가 갱신시킬 컴포넌트를 지정
	movementComp->SetUpdatedComponent(collisionComp);
	// 초기 속도
	movementComp->InitialSpeed = 5000.f;
	// 최대 속도
	movementComp->MaxSpeed = 5000.f;
	// 반동 여부
	movementComp->bShouldBounce = true;
	// 반동 값(탄성 0~1)
	movementComp->Bounciness = 0.3f;
	
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

