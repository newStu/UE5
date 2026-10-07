// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/MySceneComponent.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"


// =============================================================================
// AEnemyCharacter —— 敌人角色
//
// 功能:简单的"视线检测"AI
//   每帧向玩家发射一条射线(LineTrace),
//   如果射线没有被障碍物挡住(即"看得见"玩家),就把敌人朝向玩家。
//
// 重构说明(第 14 章):
//   视线检测逻辑已从本类下放到 UMySceneComponent 组件(LookComponents),
//   Actor 只负责:构造时创建组件、BeginPlay 时注入目标、Tick 时读结果。
//   旧的 LineTraceActor / CanSeeActor 保留为注释,方便对照两种写法。
//
// 调用流程:
//   BeginPlay:获取玩家角色作为目标(TargetCharacter),
//             并调用 LookComponents->SetTargetActor 注入给组件
//   Tick:每帧读取 LookComponents->CanSeeTargetActor() 获取检测结果
//     └─ 组件内部 TickComponent:LineTrace(检测+转身)
//          └─ CanSeeActor(const):真正执行射线检测,返回是否被遮挡
// =============================================================================


class MySceneComponent;

UCLASS()
class MYTHIRD_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// 构造函数:设置该角色类的默认属性
	AEnemyCharacter();

	// 视线检测入口:
	// 1. 以自身位置为起点、目标位置为终点
	// 2. 调用 CanSeeActor 判断中间是否有障碍物
	// 3. 看得见 -> 用 FindLookAtRotation 把自身转向目标,返回 true
	//    看不见 -> 返回 false
	// bool LineTraceActor(const AActor* TargetActor);

	// 纯检测函数:判断从 Start 射向 End 的射线是否畅通
	// 返回 true = 没有被挡住(看得见目标)
	// 声明为 const:表示该函数不修改对象状态,只做只读查询
	// bool CanSeeActor(const AActor* TargetActor, FVector Start, FVector End) const;

protected:
	// 游戏开始或 Actor 被生成时调用一次,用于初始化
	virtual void BeginPlay() override;

private:
	// 追踪的目标角色(默认为玩家角色,在 BeginPlay 中获取)
	TObjectPtr<ACharacter> TargetCharacter;

	// 本帧是否看得见玩家(每帧在 Tick 中更新)
	bool bCanSeePlayer = false;

	// CanSeeActor 是 const 成员函数,成员变量在其中是只读的,
	// 加 mutable 才能传给 LineTraceMultiByChannel 的 TArray<FHitResult>& 出参
	UPROPERTY(EditAnywhere)
	mutable TArray<FHitResult> HitResults;
	
	// 视线检测组件:检测与转身逻辑都封装在组件里(见 Components/MySceneComponent)
	// Actor 不再自己写 LineTrace,只通过 SetTargetActor 注入目标、
	// 通过 CanSeeTargetActor() 读取每帧的检测结果
	// EditAnywhere:可在蓝图/编辑器里把组件换成别的实例(通常保持默认即可)
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMySceneComponent> LookComponents;

public:
	// 每帧调用,驱动视线检测逻辑
	virtual void Tick(float DeltaTime) override;

	// 绑定输入(敌人暂无输入逻辑,仅为模板默认实现)
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
