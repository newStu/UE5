// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnim.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


/** 动画初始化:获取拥有该动画实例的 Pawn 并尝试转换为玩家角色,成功后缓存其移动组件 */
void UPlayerAnim::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// TryGetPawnOwner 返回拥有该 AnimInstance 的 Pawn(动画蓝图挂在它的 Mesh 上)
	PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());

	if (PlayerCharacter)
	{
		// 缓存移动组件引用,供后续每帧更新时直接读取速度等数据
		PlayerCharacterMovement = PlayerCharacter -> GetCharacterMovement();
	}
}


/** 每帧更新:从移动组件读取水平速度并写入 Speed,动画蓝图据此混合 Idle/Walk/Run */
void UPlayerAnim::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (PlayerCharacterMovement)
	{
		// VSizeXY 只取 XY 平面的速度长度,忽略垂直分量(跳跃/下落不影响移动动画混合)
		Speed = UKismetMathLibrary::VSizeXY(PlayerCharacterMovement -> Velocity);
	}
}
