// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/MySceneComponent.h"

// 构造函数:设置默认值
AEnemyCharacter::AEnemyCharacter()
{
	// 允许该 Actor 每帧调用 Tick()
	// 如果不需要每帧更新逻辑,可以关掉以节省性能
	PrimaryActorTick.bCanEverTick = true;

	// 创建视线检测组件(只能在构造函数里用 CreateDefaultSubobject)
	// 名字用 TEXT() 显式指定,编辑器组件树里显示"Look Components"
	LookComponents = CreateDefaultSubobject<UMySceneComponent>(TEXT("Look Components"));
	// 挂到根组件(胶囊体)上:组件有变换,射线起点 = 组件世界位置,
	// 想让"眼睛"长在别处,直接在编辑器里拖组件的相对位置即可
	LookComponents->SetupAttachment(RootComponent);
}

// 游戏开始或生成时调用:初始化追踪目标
void AEnemyCharacter::BeginPlay()
{
	// 先调用父类版本,完成 Character 基础初始化
	Super::BeginPlay();

	// 获取世界中的第 0 号玩家角色作为追踪目标
	// UGameplayStatics::GetPlayerCharacter 是全局静态工具函数,
	// 此处写在 BeginPlay 而不是构造函数里,因为构造函数阶段世界尚未完全初始化
	TargetCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);

	// 把目标注入给组件:组件只负责检测,"看谁"由 Actor 决定
	// 写在 BeginPlay 是因为此时玩家角色才刚获取到(构造函数阶段拿不到)
	LookComponents->SetTargetActor(TargetCharacter);
}

void AEnemyCharacter::Fire()
{
	if (BallProjectileClass == nullptr)
	{
		return;
	}

	FVector ForwardVector = GetActorForwardVector();
	float SpawnDistance = 40.f;
	FVector SpawnLocation = GetActorLocation() + ForwardVector * SpawnDistance;
	// GetWorld()->SpawnActor<ABallProjectile>(BallProjectileClass, SpawnLocation, GetActorRotation());

	FTransform SpawnTransform(GetActorRotation(), SpawnLocation);
	ABallProjectile *Projectile = GetWorld()->SpawnActorDeferred<ABallProjectile>(BallProjectileClass, SpawnTransform);

	Projectile->GetProjectileMovementComponent()->InitialSpeed = 1000.f;
	Projectile->FinishSpawning(SpawnTransform);
}

// 每帧调用:检测视线,看得见玩家就打日志
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 每帧读取组件的检测结果(检测本身在组件的 TickComponent 里已完成,
	// 这里只是读缓存标志,不会重复发射射线)
	bCanSeePlayer = LookComponents->CanSeeTargetActor();

	// 旧写法(逻辑已移入组件,保留对照):
	// bCanSeePlayer = LineTraceActor(TargetCharacter);

	if (bCanSeePlayer != bPreviousCanSeePlayer)
	{
		if (bCanSeePlayer)
		{
			GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AEnemyCharacter::Fire, FireInterval, true, FireDelay);
		}
		else
		{
			GetWorldTimerManager().ClearTimer(FireTimerHandle);
		}
		// 看得见玩家时输出警告日志(日志类别 LogTemp,可通过 *GetName() 打印目标名)
		// UE_LOG(LogTemp, Warning, TEXT("Can See Player %s"), *TargetCharacter->GetName());
		// Fire();
	}

	bPreviousCanSeePlayer = bCanSeePlayer;
}

// 绑定输入:敌人暂时没有输入逻辑
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// 视线检测入口:看得见目标就把自身转向目标
// bool AEnemyCharacter::LineTraceActor(const AActor* TargetActor)
// {
// 	// 目标为空(比如玩家还没生成)直接返回看不见
// 	if (TargetActor == nullptr)
// 	{
// 		return false;
// 	}
//
// 	// 射线起点:敌人自身位置
// 	FVector Start = GetActorLocation();
// 	// 射线终点:目标(玩家)位置
// 	FVector End = TargetActor->GetActorLocation();
//
// 	// 先判断能不能看见(射线是否被障碍物挡住)
// 	if (CanSeeActor(TargetActor, Start, End))
// 	{
// 		// 看得见:计算"从起点看向终点"所需的旋转角
// 		// FindLookAtRotation 是 Kismet 数学库中的工具函数,
// 		// 返回让 X 轴指向目标方向的 FRotator
// 		FRotator DirectRotation = UKismetMathLibrary::FindLookAtRotation(Start, End);
// 		// 把敌人整体转向该方向
// 		SetActorRotation(DirectRotation);
// 		return true;
// 	}
//
// 	// 看不见:不做任何转向
// 	return false;
// }

// 纯检测:判断从 Start 到 End 的射线是否畅通(畅通 = 看得见)
// bool AEnemyCharacter::CanSeeActor(const AActor* TargetActor, FVector Start, FVector End) const
// {
// 	if (TargetActor == nullptr)
// 	{
// 		return false;
// 	}
//
// 	// 单次检测命中结果(LineTraceSingleByChannel 会填充它)
// 	FHitResult Hit;
// 	// 使用 Visibility 通道:能被"看见"的几何体都会响应该通道
// 	ECollisionChannel Channel = ECollisionChannel::ECC_Visibility;
//
//
// 	// —— 自定义 Trace Channel(备用,想用时放开下面这行)——
// 	// 在 项目设置 -> Collision 中新增通道后,Config/DefaultEngine.ini 会多出一行:
// 	//   +DefaultChannelResponses=(Channel=ECC_GameTraceChannel1,
// 	//                             DefaultResponse=ECR_Block, bTraceType=True,
// 	//                             Name="MyChannelBlock")
// 	// 各字段含义:
// 	//   bTraceType=True  -> 是"射线(Trace)"类型通道;蓝图中显示为 TraceTypeQuery1,
// 	//                       C++ 中对应的枚举值就是 ECC_GameTraceChannel1
// 	//                       (若设为 False 则是 Object 通道,蓝图中是 ObjectTypeQuery1)
// 	//   DefaultResponse=ECR_Block -> 新通道对所有物体默认"阻挡",
// 	//                       即默认情况下任何物体都会挡住这条射线
// 	// 注意:每个自定义通道的枚举值按添加顺序递增(GameTraceChannel1/2/3...),
// 	//       中途删除或调整顺序会使已有枚举值错位,需同步检查所有引用处
// 	// ECollisionChannel Channel = ECollisionChannel::ECC_GameTraceChannel1;
//
// 	// 忽略玩家和敌人
// 	// 如果不忽略自身和目标,射线会先打中两者的碰撞体,导致永远"看不见"
// 	FCollisionQueryParams QueryParams;
// 	QueryParams.AddIgnoredActor(this);
// 	QueryParams.AddIgnoredActor(TargetActor);
//
//
// 	// 单射线检测:只记录第一个 blocking 命中结果到 Hit
// 	GetWorld() -> LineTraceSingleByChannel(Hit, Start, End, Channel, QueryParams);
// 	// 多射线检测:记录沿途所有命中结果到 HitResults(练习用,当前逻辑只依赖上面的 Hit)
// 	// 重要:Multi 射线返回的是"沿途所有 Overlap 命中 + 到第一个 Block 命中为止",
// 	//       即射线碰到 Block 响应的物体就会截止,后面的物体不会进入 HitResults。
// 	//       想让射线"穿过"并记录沿途每个物体,需要把物体对所用通道的响应设为 Overlap:
// 	//       例如在静态网格体的 Collision Preset 里选 OverlapAll,
// 	//       或单独把该物体对 Visibility / MyChannelBlock 的响应改为 Overlap。
// 	//       (物体对通道的响应为 Ignore 时,Multi 射线也会直接无视它)
// 	GetWorld() -> LineTraceMultiByChannel( HitResults, Start, End, Channel, QueryParams);
//
// 	// 画出调试射线(蓝色),false = 不持久(只显示一帧),便于在视口中直观查看视线
// 	DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false);
//
//
// 	// Hit.bBlockingHit == true 说明射线被某个障碍物挡住(看不见)
// 	// 取反:没有被挡住 => 看得见目标
// 	return !Hit.bBlockingHit;
// }
