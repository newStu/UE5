// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyThirdCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

// 定义日志类别(与头文件中的 DECLARE 对应)
DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AMyThirdCharacter

// 构造函数:设置角色默认参数与相机组件
AMyThirdCharacter::AMyThirdCharacter()
{
	// Set size for collision capsule
	// 设置碰撞胶囊体的半径与半高(决定角色的碰撞体型)
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	// 角色本体不跟随控制器旋转(不做第一人称式转身),
	// 控制器的旋转只影响相机,角色朝向由移动方向决定(见下方 bOrientRotationToMovement)
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	// 配置角色移动组件
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	// 角色朝移动方向转身(面朝移动方向,而不是面朝相机方向)
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate
	// 转身角速度:每秒绕 Yaw 转 500 度(Pitch/Roll 为 0,即只在水平面内转身)

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	// 提示:以下这些数值参数也可以直接在角色蓝图里改,不必重新编译 C++,迭代更快
	GetCharacterMovement()->JumpZVelocity = 700.f;              // 跳跃初速度(决定跳多高)
	GetCharacterMovement()->AirControl = 0.35f;                 // 空中操控系数(0~1,越大空中越容易改变方向)
	GetCharacterMovement()->MaxWalkSpeed = 500.f;               // 最大行走速度
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;          // 摇杆最小走路速度(轻推摇杆时的最慢速度)
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;// 行走刹车间速度(松开输入后多快停下来)
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;// 下落刹车间速度(限制水平方向漂移)

	// Create a camera boom (pulls in towards the player if there is a collision)
	// 创建相机臂(若碰到障碍物会自动缩短,避免相机穿墙)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	// 挂到根组件(胶囊体)上
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 1000.0f; // The camera follows at this distance behind the character
	// 相机臂长度:相机在角色后方 1000 单位处(数值越大镜头拉得越远)
	CameraBoom->bUsePawnControlRotation = false; // Rotate the arm based on the controller
	// 相机臂不跟随控制器旋转(本项目采用固定俯视角,旋转由下面这行直接设定)
	CameraBoom->SetRelativeRotation(FRotator(-80.0f, 0.0f, 0.0f));
	// 相机臂俯角 -80°:接近正俯视的视角(负 Pitch = 向下看)
	CameraBoom->bInheritPitch = false;  // 不继承控制器的俯仰
	CameraBoom->bInheritRoll = false;   // 不继承控制器的翻滚
	CameraBoom->bInheritYaw = false;    // 不继承控制器的偏航(保证视角始终固定)

	// Create a follow camera
	// 创建跟随相机
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	// 把相机挂到相机臂末端的插槽上(SocketName 是 SpringArm 预定义的末端挂点)
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	// 相机自身不再额外跟随控制器旋转,完全由相机臂决定朝向

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content reference in C++)
	// 提示:骨骼网格体与动画蓝图不在 C++ 里引用,
	// 而是在派生蓝图 ThirdPersonCharacter(BP_ThirdPersonCharacter)中指定,避免 C++ 直接依赖资产

}

// 游戏开始:目前仅调用父类实现
void AMyThirdCharacter::BeginPlay()
{
	// Call the base class
	Super::BeginPlay();
}

//////////////////////////////////////////////////////////////////////////
// Input

// 输入绑定:添加映射上下文 + 绑定各 InputAction 的回调
void AMyThirdCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	// 把默认映射上下文注册到本地玩家的 Enhanced Input 子系统(优先级 0)
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Set up action bindings
	// 把 InputAction 绑定到具体回调(需要把基类 InputComponent 转成 EnhancedInputComponent)
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		// 跳跃:按下(Started)起跳,松开(Completed)停止跳
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		// 移动:按住期间持续(Triggered)调用 Move
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyThirdCharacter::Move);

		// Looking
		// 视角:鼠标移动时持续调用 Look
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyThirdCharacter::Look);
	}
	else
	{
		// 找不到 Enhanced Input 组件时报错:本模板基于 Enhanced Input 构建
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

// 移动:把二维输入转成世界方向并驱动角色
void AMyThirdCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	// 输入是一个二维向量:X = 右方向键,D = 前方向键
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		// 取控制器(玩家视角)的旋转,并只保留 Yaw(去掉俯仰,保证移动贴地)
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		// 由 Yaw 旋转矩阵取出"前方"单位向量
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector
		// 同理取出"右方"单位向量
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement
		// 组合移动输入:Y 轴输入沿前方、X 轴输入沿右方,叠加到角色移动
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

// 视角:把输入增量交给控制器旋转
void AMyThirdCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	// 输入为二维向量:X = 左右移动,Y = 上下移动
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		// 把增量加给控制器:X 控制偏航(左右看),Y 控制俯仰(上下看)
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
