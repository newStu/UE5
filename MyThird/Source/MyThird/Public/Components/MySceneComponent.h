// Fill out your copyright notice in the Description page of Project Settings.
// =============================================================================
// 自定义场景组件:视线检测组件
//
// 由第 13 章 EnemyCharacter 里的 LineTraceActor / CanSeeActor 重构而来:
// 把"看得到目标吗 + 看到就转身"这套逻辑从 Actor 本体抽到组件里,
// 好处是任何 Actor(不只敌人)挂上这个组件就能获得视线检测能力。
//
// 为什么继承 USceneComponent 而不是 UActorComponent:
// 射线起点需要"摆在世界中的某个位置"(GetComponentLocation),
// 有位置就得有变换(Transform),所以要选 Scene 系基类。
// 想改变检测起点,直接在编辑器里拖动组件的相对位置即可,不用改代码。
// =============================================================================

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "MySceneComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYTHIRD_API UMySceneComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	// 构造函数:设置组件默认值
	UMySceneComponent();

protected:
	// 游戏开始或组件所属 Actor 被生成时调用一次,用于初始化
	virtual void BeginPlay() override;

	// 视线检测入口:看得见目标就把拥有者(GetOwner)转向目标
	// 返回 true = 看得见(并已完成转身),false = 看不见或目标为空
	bool LineTrace();

	// 纯检测函数:判断从 Start 射向 End 的射线是否畅通
	// IgnoreActors:射线要忽略的 Actor 列表(至少包含拥有者和目标自身,
	// 否则射线会先打中两者的碰撞体,导致永远"看不见")
	// 声明为 const:表示该函数不修改对象状态,只做只读查询
	bool CanSeeActor(FVector Start, FVector End, TArray<const AActor*>& IgnoreActors) const;

	// 追踪目标(比如玩家角色),由拥有者在 BeginPlay 里通过 SetTargetActor 注入
	// 组件自己不去查找玩家:保持"组件只负责检测,目标是谁由外界决定",
	// 这样组件不依赖 GetPlayerCharacter,复用时更自由
	TObjectPtr<AActor> TargetActor;

	// 本帧的检测结果缓存,外界通过 CanSeeTargetActor() 读取,
	// 避免 Actor 每帧再调一次检测函数
	bool bCanSee = false;

public:
	// 每帧调用,驱动视线检测逻辑
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 获取目标:FORCEINLINE 内联访问器,零调用开销
	// 应在拥有者的 BeginPlay 里调用(那时玩家角色已生成就绪)
	FORCEINLINE void SetTargetActor(AActor* Actor)
	{
		TargetActor = Actor;
	}

	// 返回最近一帧的检测结果(不重新检测,只读缓存)
	FORCEINLINE bool CanSeeTargetActor()
	{
		return bCanSee;
	}

};
