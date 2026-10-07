// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BallProjectile.generated.h"

// 球体投射物:敌人 Fire() 生成的一次性子弹
// 组成:球体碰撞组件(模拟物理,负责碰撞/重力/弹跳)
//     + ProjectileMovementComponent(仅负责把初速度交给刚体,之后物理引擎接管)
// 行为:命中玩家自我销毁;4 秒生命周期兜底防止飞出场外堆积
UCLASS()
class MYTHIRD_API ABallProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	// 构造函数:创建组件,配置碰撞预设/模拟物理/初速度(实现见 .cpp)
	ABallProjectile();

protected:
	// Called when the game starts or when spawned
	// 生成时调用:绑定 OnHit 回调 + 设置生命周期
	virtual void BeginPlay() override;

private:
	// 球体碰撞组件(根组件):既是碰撞体,也是模拟物理的刚体
	// private + AllowPrivateAccess:外部不能直接改指针,但编辑器/蓝图里仍可见
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ball", meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> SphereComponent;

	// 投射物移动组件:与模拟物理是"交接"关系 ——
	// 生成时按 InitialSpeed × 生成朝向算出初速度交给刚体,随后每帧不再驱动
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ball", meta = (AllowPrivateAccess = true))
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

public:
	// 只读访问器:供外部(如发射者)查询移动组件(初速度等属性)
	FORCEINLINE UProjectileMovementComponent* GetProjectileMovementComponent() const
	{
		return ProjectileMovementComponent;
	}

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 命中回调:OnComponentHit 动态委托的响应函数,模拟物理撞到东西时触发
	// 必须标 UFUNCTION():AddDynamic 绑定要求函数能被反射系统按名字查找
	// 参数:HitComponent=被撞的组件(自己)  OtherActor=撞到的 Actor
	//       OtherComp=撞到的组件  NormalImpulse=法线冲量(碰撞强度)  Hit=完整命中信息
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
