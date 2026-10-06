// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerAnim.generated.h"

// 前置声明:头文件中只用到了 APlayerCharacter 的指针,无需完整包含其头文件
class APlayerCharacter;

/**
 * 玩家角色动画实例类
 * 负责驱动动画蓝图的 C++ 逻辑层:
 * 在初始化时缓存角色及其移动组件引用,每帧更新动画所需的状态数据(如移动速度),
 * 供动画蓝图中的状态机 / 混合空间(Blend Space)根据这些值切换 Idle / Walk / Run 等动画
 */
UCLASS()
class MYTHIRD_API UPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	/** 动画初始化时调用(仅一次),用于缓存角色和移动组件的引用,避免每帧重复获取 */
	virtual void NativeInitializeAnimation() override;

	/** 每帧调用(动画蓝图 Update Animation 事件的 C++ 版本),用于刷新动画状态变量 */
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	/** 拥有该动画实例的玩家角色引用(从 TryGetPawnOwner 转换而来),只读暴露给蓝图 */
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<APlayerCharacter> PlayerCharacter;

	/** 玩家角色的移动组件引用,用于读取速度等运动数据,只读暴露给蓝图 */
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UCharacterMovementComponent> PlayerCharacterMovement;

	/** 角色在水平面(XY 平面)上的移动速度,用于在混合空间中按速度混合不同动画 */
	UPROPERTY(BlueprintReadOnly, Category="CPlus")
	float Speed = 0.f;
};
