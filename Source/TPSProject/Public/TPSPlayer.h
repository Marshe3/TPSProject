// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "TPSPlayer.generated.h"

class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class USpringArmComponent;

UCLASS()
class TPSPROJECT_API ATPSPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// 스프링 암 컴포넌트 선언
	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<USpringArmComponent> springArmComp;
	
	// 카메라 컴포넌트 선언
	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<UCameraComponent> cameraComp;
	
	// 총 스켈레탈 메시 컴포넌트 선언
	UPROPERTY(VisibleAnywhere, Category = "GetMesh")
	class USkeletalMeshComponent* gunMeshComp;
	
	// IMC 선택 필드 선언
	UPROPERTY(EditDefaultsOnly, Category = Input)
	TObjectPtr<UInputMappingContext> imc_TPS;
	
	// IA 선택 필드 선언
	UPROPERTY(EditDefaultsOnly, Category = Input)
	TObjectPtr<UInputAction> ia_LookUp;
	
	UPROPERTY(EditDefaultsOnly, Category = Input)
	TObjectPtr<UInputAction> ia_Turn;
	
	// 키보드로 점프 IA 필드 선언
	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* ia_jump;
	
	//  전후좌우 키보드 이동 IA 필드 선언
	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* ia_Move;
	
	// 마우스 발사 IA 필드 선언
	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* ia_Fire;
	
	// 이동 속도
	UPROPERTY(EditDefaultsOnly, Category = Input)
	float walkSpeed = 600.f;

	// 총알 스폰 팩토리
	UPROPERTY(EditDefaultsOnly, Category = BulletFactory)
	TSubclassOf<class ABullet> bulletFactory;
	
	// 이동 방향
	FVector direction;
	
	// 전후좌우 이동 입력 함수 선언
	void Move(const struct FInputActionValue& inputValue);
	
	// 상하 회전 입력 함수 선언
	void LookUp(const FInputActionValue& inputValue);
	
	// 좌우 회전 입력 함수 선언
	void Turn(const FInputActionValue& inputValue); 
	
	//점프 입력 함수 선언
	void InputJump(const struct FInputActionValue& inputValue);
	
	// 총알 발사 입력 함수 선언
	void InputFire(const struct FInputActionValue& inputValue);
};
