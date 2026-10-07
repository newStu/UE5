// Fill out your copyright notice in the Description page of Project Settings.
// =============================================================================
// 视线检测组件实现
// 逻辑与第 13 章 EnemyCharacter 版本一致,区别只有两处:
//   1. 射线起点从 GetActorLocation() 换成 GetComponentLocation()
//      —— 组件有自己的位置,检测起点跟随组件而非 Actor 根
//   2. 转身操作通过 GetOwner()->SetActorRotation() 间接完成
//      —— 组件不能直接 SetActorRotation,它只是 Actor 身上的一个部件
// =============================================================================


#include "Components/MySceneComponent.h"

#include "Kismet/KismetMathLibrary.h"

// 构造函数:设置组件默认值
UMySceneComponent::UMySceneComponent()
{
	// 开启组件 Tick:视线检测需要每帧执行
	// (不需要每帧逻辑的组件应设为 false 省性能)
	PrimaryComponentTick.bCanEverTick = true;
}


// 游戏开始或生成时调用:目前无额外初始化,目标由拥有者注入
void UMySceneComponent::BeginPlay()
{
	Super::BeginPlay();
}




// 视线检测入口:看得见目标就把拥有者转向目标
bool UMySceneComponent::LineTrace()
{
	// 目标为空(比如还没人调用 SetTargetActor)直接返回看不见
	if (TargetActor == nullptr)
	{
		return false;
	}

	// 射线起点:组件自身位置(而非 Actor 位置)
	// 组件挂在 Actor 不同位置,检测起点就跟着变 —— 这就是用 SceneComponent 的意义
	FVector Start = GetComponentLocation();
	// 射线终点:目标(玩家)位置
	FVector End = TargetActor->GetActorLocation();

	// 忽略拥有者自身和目标:否则射线会先打中两者的碰撞体,导致永远"看不见"
	TArray<const AActor*> IgnoreActors = {GetOwner(), TargetActor};
	if (CanSeeActor(Start, End, IgnoreActors))
	{
		// 看得见:计算"从起点看向终点"所需的旋转角
		// FindLookAtRotation 返回让 X 轴指向目标方向的 FRotator
		FRotator DirectRotation = UKismetMathLibrary::FindLookAtRotation(Start, End);
		// 组件本身不能转身,通过 GetOwner() 让整个 Actor 转向该方向
		GetOwner() -> SetActorRotation(DirectRotation);
		return true;
	}

	// 看不见:不做任何转向
	return false;
}


// 纯检测:判断从 Start 到 End 的射线是否畅通(畅通 = 看得见)
bool UMySceneComponent::CanSeeActor(FVector Start, FVector End, TArray<const AActor*>& IgnoreActors) const
{
	// 双重保险:目标为空直接返回
	if (TargetActor == nullptr)
	{
		return false;
	}

	// 单次检测命中结果(LineTraceSingleByChannel 会填充它)
	FHitResult Hit;
	// 使用 Visibility 通道:能被"看见"的几何体都会响应该通道
	ECollisionChannel Channel = ECollisionChannel::ECC_Visibility;

	// 查询参数:把拥有者和目标加入忽略列表
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActors(IgnoreActors);

	// 单射线检测:只记录第一个 blocking 命中结果到 Hit
	GetWorld() -> LineTraceSingleByChannel(Hit, Start, End, Channel, QueryParams);

	// 画出调试射线(蓝色),false = 不持久(只显示一帧),便于在视口中直观查看视线
	DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false);

	// Hit.bBlockingHit == true 说明射线被某个障碍物挡住(看不见)
	// 取反:没有被挡住 => 看得见目标
	return !Hit.bBlockingHit;
}


// 每帧调用:执行检测并缓存结果,外界通过 CanSeeTargetActor() 读取
void UMySceneComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// 每帧做一次视线检测,更新缓存标志
	bCanSee = LineTrace();
}
