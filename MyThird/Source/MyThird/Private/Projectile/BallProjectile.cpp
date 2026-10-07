// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile/BallProjectile.h"

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
	
	SetLifeSpan(4.f);
}

// Called every frame
void ABallProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

