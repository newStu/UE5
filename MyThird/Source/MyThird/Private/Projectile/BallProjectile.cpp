// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile/BallProjectile.h"
// OnHit 里要 Cast 成玩家角色类,必须包含其头文件
#include "MyThird/MyThirdCharacter.h"

// Sets default values
ABallProjectile::ABallProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>("Sphere Component");
	SphereComponent->SetSphereRadius(35.f); 
	
	// 对 Visibility 通道忽略:视线检测(LineTrace)用的是 ECC_Visibility,
	// 子弹若响应该通道会挡住敌人自己的射线,导致"发射 → 看不见 → 停火"的振荡
	// SphereComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	
	// 碰撞预设:设置我们自定义的碰撞预设
	SphereComponent->SetCollisionProfileName("Ball");
	// 模拟onHit
	// 物理通道必须开着
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereComponent->SetSimulatePhysics(true);   // 开启模拟物理 
	SphereComponent->SetNotifyRigidBodyCollision(true);
	
	SetRootComponent(SphereComponent);
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("Projectile Movement Component");
	ProjectileMovementComponent->InitialSpeed = 1000.f;
}

// Called when the game starts or when spawned
void ABallProjectile::BeginPlay()
{
	Super::BeginPlay();

	// 动态委托绑定:模拟物理撞到东西时回调 OnHit
	// 前提:组件开启 SetNotifyRigidBodyCollision(true)(见构造函数)
	SphereComponent->OnComponentHit.AddDynamic(this, &ABallProjectile::OnHit);
	// 生命周期兜底:4 秒后自动 Destroy,防止没撞到玩家的子弹飞出场外堆积
	SetLifeSpan(4.f);
}

// Called every frame
void ABallProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// 命中回调:撞到东西时触发(绑定见 BeginPlay)
// 只关心"撞的是不是玩家":是 → 打日志 + 自我销毁;撞到墙/地面等则忽略,继续弹跳
void ABallProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 把撞到的 Actor 尝试转换成玩家角色类:转换成功说明命中玩家
	AMyThirdCharacter* Player = Cast<AMyThirdCharacter>(OtherActor);
	if (Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit"));
		// 命中玩家,子弹一次性消亡
		Destroy();
	}
};
