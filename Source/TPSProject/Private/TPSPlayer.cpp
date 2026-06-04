// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayer.h"

#include "Bullet.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NiagaraFunctionLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


// Sets default values
ATPSPlayer::ATPSPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// TPS 카메라를 SpringArm 컴포넌트에 부착
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	springArmComp->SetupAttachment(RootComponent); // 계층 구조상 캡슐컴포넌트가 ROOT이며 스프링암을 자식으로 배치
	springArmComp->SetRelativeLocation(FVector(.0f, 70.0f, 90.0f)); // 암 컴포넌트의 시작점
	springArmComp->TargetArmLength = 400.f;
	
	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	cameraComp->SetupAttachment(springArmComp);
	
	// // C++에서 BP에서의 옵션들 직접 수정하는 경우 아래처럼 해당 옵션 변수들을 직접 코드로 제어 가능
	// springArmComp->bUsePawnControlRotation = true;
	// cameraComp->bUsePawnControlRotation = false;
	// bUseControllerRotationYaw = false;
	// GetCharacterMovement()->bOrientRotationToMovement = true;
	
	// 총 스켈레탈메시 컴포넌트 등록
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComponent"));
	// 캐릭터 메시 컴포넌트(GetMesh()) 부모에 부착
	gunMeshComp->SetupAttachment(GetMesh());
	// LIneTrace가 총에 막히지 않도록 충돌 해제
	gunMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// 스켈레탈 메시 데이터 동적로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Weapons/GrenadeLauncher/Meshes/SKM_GrenadeLauncher.SKM_GrenadeLauncher'"));
	if (TempGunMesh.Succeeded())
	{
		// 해당 경로의 스켈레탈메시를 찾았다면, 메시 할당 + 임시위치 보정
		gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);
		gunMeshComp->SetRelativeLocation(FVector(-14.0f, 52.0f, 120.0f));
		
	}
	
	// 스나이퍼건 스태틱 메시 컴포넌트 등록 
	sniperGunComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SniperGun StaticMeshComponent"));
	// 캐릭터 메시 컴포넌트(GetMesh()) 부모에 qnckr
	sniperGunComp->SetupAttachment(GetMesh());
	// LIneTrace가 총에 막히지 않도록 충돌 해제
	sniperGunComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempSniperGunMesh(TEXT("/Script/Engine.StaticMesh'/Game/Weapons/Sniper/Meshes/sniper1.sniper1'"));
	if (TempSniperGunMesh.Succeeded())
	{
		// 해당 경로의 스켈레탈메시를 찾았다면, 메시 할당 + 임시 위치보정
		sniperGunComp->SetStaticMesh(TempSniperGunMesh.Object);
		sniperGunComp->SetRelativeLocation(FVector(-14.0f, 52.0f, 120.0f));
		sniperGunComp->SetRelativeScale3D(FVector(0.8f));
	}
	
	// 시작 시 기본 무기로 스나이퍼건을(유탄총을 숨김)
	bUsingGrenadeGun = false;
	sniperGunComp->SetVisibility(true);
	gunMeshComp->SetVisibility(false);
}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
	
	// Enhanced Input 시스템이 IMC_TPS 사용하도록 설정
	auto pc = Cast<APlayerController>(Controller);
	if (pc)
	{
		auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if (subsystem)
		{
			subsystem->AddMappingContext(imc_TPS, 0);
		}
	}
	
	// 스나이퍼 UI 위젯 인스턴스 생성 (화면에 보이기 위해서는 AddTOViewport() 호출시 등장)
	sniperUI = CreateWidget(GetWorld(), sniperUIFactory);
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	auto PlayerInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (PlayerInput)
	{
		PlayerInput->BindAction(ia_LookUp, ETriggerEvent::Triggered, this, &ATPSPlayer::LookUp);
		PlayerInput->BindAction(ia_Turn, ETriggerEvent::Triggered, this, &ATPSPlayer::Turn);
		PlayerInput->BindAction(ia_Move, ETriggerEvent::Triggered, this, &ATPSPlayer::Move);
		PlayerInput->BindAction(ia_jump, ETriggerEvent::Started, this, &ATPSPlayer::InputJump);
		PlayerInput->BindAction(ia_Fire, ETriggerEvent::Started, this, &ATPSPlayer::InputFire);
		PlayerInput->BindAction(ia_GrenadeGun, ETriggerEvent::Started, this, &ATPSPlayer::ChangeToGrenadeGun);
		PlayerInput->BindAction(ia_SniperGun, ETriggerEvent::Started, this, &ATPSPlayer::ChangeToSniperGun);
		PlayerInput->BindAction(ia_SniperZoom, ETriggerEvent::Started, this, &ATPSPlayer::SniperZoom);
		//PlayerInput->BindAction(ia_SniperZoom, ETriggerEvent::Completed, this, &ATPSPlayer::SniperZoom); // 토글 방식 사용시
	}
}

// 상하 회전 입력에 따른 콜백 함수 구현
void ATPSPlayer::LookUp(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	AddControllerPitchInput(value); // PITCH(Y축) 회전
}

// 좌우 회전 입력에 따른 콜백 함수 구현
void ATPSPlayer::Turn(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	AddControllerYawInput(value); // YAW(Z축) 회전
}

// 전후좌우 이동 이력에 따른 콜백 함수 구현
void ATPSPlayer::Move(const FInputActionValue& inputValue)
{
	FVector2D value = inputValue.Get<FVector2D>(); // 전달받는 2D 값
	
	// 카메라의 좌우 회전(Yaw)만 사용해서 이동 방향을 만든다.
	const FRotator ControlRot = GetControlRotation();
	const FRotator YawRot(0.0f, ControlRot.Yaw, 0.0f);
	const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

	AddMovementInput(Forward, value.X); // 전후
	AddMovementInput(Right, value.Y); // 좌우
}

// 점프 입력에 따른 롤백 함수 구현
void ATPSPlayer::InputJump(const FInputActionValue& inputValue)
{
	Jump();
}

void ATPSPlayer::InputFire(const FInputActionValue& inputValue)
{
	if (bUsingGrenadeGun)
	{
		// 유탄총을 사용하는 경우
		// 총 스켈레탈메시에 FirePosition이란 이름의 소켓의 월드 트랜스폼(위치/회전)을 가져옴
		FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
		// 위 위치/회전으로 BulletFactory가 BP_Bullet 인스턴스를 월드에 스폰
		GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);
	}
	else
	{
		// 스나이퍼건을 사용하는 경우
		// 라인의 시작/종료 위치 설정 - 카메라부터 카메라 정면 50m까지
		FVector startPos = cameraComp->GetComponentLocation();
		FVector endPos = startPos + cameraComp->GetForwardVector() * 5000.f;  // 50m
		
		// 충돌 결과 저장, 자기자신은 충돌 검사에서 제외
		FHitResult hitResult;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);
		
		// LineTraceSingleByChannel(결과그릇, 시작위치, 종료위치, 트레이스채널, 충돌옵션)
		// Visibility 채널로 라인트레이스를 실행하고 -> 처음 부딪힌 액터 하나만 검출
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, startPos, endPos, ECC_Visibility, params);
		
		// [DEBUG] LineTrace 경로 시각화
		DrawDebugLine(GetWorld(), startPos, endPos, bHit ? FColor::Green : FColor::Red, false, 1.f, 0, .2f);
		
		if (bHit)
		{
			// [DEBUG] 충돌정보
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor = %s, Component = %s, Distance = %.1f, ImpactPoint = %s"),
				hitResult.GetActor() ? *hitResult.GetActor()->GetName() : TEXT("None"),
				hitResult.GetComponent() ? *hitResult.GetComponent()->GetName() : TEXT("None"),
				hitResult.Distance,
				*hitResult.ImpactPoint.ToString()
				);
			
			// [DEBUG] 타격 위치 시각화
			DrawDebugSphere(GetWorld(), hitResult.ImpactPoint, 20.f, 12, FColor::Yellow, false, 2.f);
			
			// 타격 위치에 Niagara 이펙트 스폰
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), bulletEffectFactory, hitResult.ImpactPoint);
		}
	}
	
	
}

void ATPSPlayer::ChangeToGrenadeGun(const struct FInputActionValue& inputValue)
{
	// 사용 중 플래그를 유탄총으로 변경
	bUsingGrenadeGun = true;
	// 스나이퍼 숨기고 / 유탄총 보이게
	sniperGunComp->SetVisibility(false);
	gunMeshComp->SetVisibility(true);
}

void ATPSPlayer::ChangeToSniperGun(const struct FInputActionValue& inputValue)
{
	// 사용 중 플래그를 유탄총으로 변경
	bUsingGrenadeGun = false;
	// 스나이퍼 숨기고 / 유탄총 보이게
	sniperGunComp->SetVisibility(true);
	gunMeshComp->SetVisibility(false);
}

void ATPSPlayer::SniperZoom()
{
	// 스나이퍼 총이 아닐 떄는 동작하지 않음
	if (bUsingGrenadeGun)
	{
		return;
	}
	// 키 누름 - 줌 모드에 진입
	if (bSniperZoom == false)
	{
		bSniperZoom = true;
		sniperUI->AddToViewport(); // 조준경 UI 화면에 나타남
		cameraComp->SetFieldOfView(45.f); // FOV 시야각을 좁혀서 줌인 효과
		
	}
	else
	{
		// 키 해제 - 줌 모드에서 해제
		bSniperZoom = false;
		sniperUI->RemoveFromParent(); // 조준경 UI 제거
		cameraComp->SetFieldOfView(90.f); // FOV 시야각 복구

	}
	
}