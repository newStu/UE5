// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom -> SetupAttachment(RootComponent);

	CameraBoom -> TargetArmLength = 600.0f;
	
	
	// 如果需要修改
	// SetRootComponent(CameraBoom);
	// 相机附着到Boom
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCamera  -> SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// 将相机附着到指定的位置
	// PlayerCamera -> SetupAttachment(GetMesh(), FName("RootSocket"));
	
	
	// 组合，将SphereComponent设置内容后，后续可以直接使用这个进行操作
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	SphereComponent -> SetSphereRadius(400.0f);
	

	// 1.不要让角色随着控制器旋转
	// 身体不直接跟随控制器旋转，由移动组件朝移动方向转身
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;
	
	// 2.Boom 跟随控制器旋转，相机自身不读控制旋转（旋转来自挂在 Boom 末端的继承）
	// Boom 跟随控制器旋转，Look 输入才能带动相机俯仰/旋转
	CameraBoom -> bUsePawnControlRotation = true;
	PlayerCamera -> bUsePawnControlRotation = false;
	
	// 3.角色要跟据其运动方向进行旋转
	GetCharacterMovement() -> bOrientRotationToMovement = true;
	GetCharacterMovement() -> RotationRate = FRotator(0.f, 400.f, 0.f);
	
	
	
	UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::SetupAttachment()"));
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::BeginPlay()"));
	Super::BeginPlay();
	
	MyBlueprintEvent();
	
	UE_LOG(LogTemp, Warning, TEXT("显示一下报错呗"));

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow,
			FString::Printf(TEXT("打印一些特殊文字：%s"), TEXT("Hello World")));
	}
	
	const ULocalPlayer* Player = (GEngine && GetWorld()) ? GEngine -> GetFirstGamePlayer(GetWorld()) : nullptr;
	if (Player)
	{
		UEnhancedInputLocalPlayerSubsystem* SubSystem =  ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Player);
		if (DefaultMappingContext)
		{
			SubSystem -> AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void APlayerCharacter::TestFunction()
{
	UE_LOG(LogTemp, Warning, TEXT("TestFunction called with testLen: %f"), testLen);
}

bool APlayerCharacter::GetTestBoolBlueprint()
{
	return false;
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Attack);
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	// 输入值是 Vector2D：X = 前后，Y = 左右
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// // 前进/后退
		// AddMovementInput(GetActorForwardVector(), MovementVector.X);
		// // 左右
		// AddMovementInput(GetActorRightVector(), MovementVector.Y);
		
		
		const FRotator Rotation = Controller -> GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(ForwardDirection, MovementVector.X);
		AddMovementInput(RightDirection, MovementVector.Y);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// Y 轴控制左右视角，X 轴（鼠标 deltaY）控制俯仰
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}


void APlayerCharacter::Attack()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
			FString::Printf(TEXT("Attack from Character"))); 
}
