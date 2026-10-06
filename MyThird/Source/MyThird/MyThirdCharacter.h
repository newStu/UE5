// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MyThirdCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

// 声明本角色专用的日志类别(可在输出日志中按 LogTemplateCharacter 过滤)
DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AMyThirdCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	// 相机臂(SpringArm):把相机吊在角色后方的"摇臂"。
	// 它本身不渲染,只负责控制相机的位置与旋转(本项目的俯视角就由它的旋转决定)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	/** Follow camera */
	// 跟随相机:挂在相机臂末端,实际"看"画面、渲染视口的组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	/** MappingContext */
	// 默认输入映射上下文(IMC):定义"哪些按键/手柄输入 -> 哪些 InputAction"的映射表,
	// 在蓝图中指定(如 IMC_Default),运行时通过 Enhanced Input 子系统生效
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	/** Jump Input Action */
	// 跳跃输入动作:按下/松开时分别触发 Jump / StopJumping
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;

	/** Move Input Action */
	// 移动输入动作:持续触发时调用本类的 Move(移动)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;

	/** Look Input Action */
	// 视角旋转输入动作:触发时调用本类的 Look(转视角)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;

public:
	// 构造函数:初始化胶囊体尺寸、移动参数、相机臂与跟随相机
	AMyThirdCharacter();


protected:

	/** Called for movement input */
	// 移动输入回调:根据输入向量(WASD/摇杆)沿控制器 Yaw 朝向移动角色
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	// 视角输入回调:把鼠标/摇杆增量转成控制器的 Yaw(左右)/ Pitch(上下)旋转
	void Look(const FInputActionValue& Value);

protected:
	// APawn interface
	// 输入绑定入口:引擎初始化输入时调用,
	// 这里完成两件事 —— 添加映射上下文、把各个 InputAction 绑定到回调函数
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	// 游戏开始时调用(本模板中仅调用父类,未做额外逻辑)
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	// 内联访问器:获取相机臂组件(供蓝图/其他代码只读访问)
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	// 内联访问器:获取跟随相机组件
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
