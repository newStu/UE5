# 查阅：UE API 速查

与[《查阅：特殊语法速查》](查阅-特殊语法速查.md)的分工：那边管**语言与宏**（模板怎么写、断言怎么分级），这边管**引擎 API**（函数怎么用、虚函数何时被调、参数什么含义）。按**职能域**分组收录；工程自定义的函数（`FindNativeInputActionByTag` 这类）不收。

## Actor 与组件基础

- **`PrimaryActorTick.bCanEverTick` / `bStartWithTickEnabled`**（AActor）——前者彻底关掉 Tick 注册，后者"允许 Tick 但出生不启用"，留给 `SetActorTickEnabled(true)` 按需开。双关 = 宣示"无每帧逻辑"；
- **`GetMesh()`**（ACharacter）——取 SkeletalMeshComponent，构造时已保证存在，可放心裸调；
- **`bReceivesDecals`**（USkinnedMeshComponent）——false 让网格不吃贴花投影：防穿帮 + 省渲染；
- **`CreateDefaultSubobject<T>(TEXT("名"))`**——只能在构造函数里调用，为 CDO 创建组件；蓝图子类细节面板自动可见；
- **`SetupAttachment(父组件, Socket名)`**——构造期挂接，晚调用静默无效；运行时改挂接要用 `AttachToComponent`；
- **`GetRootComponent()` / `SetRootComponent()`**——取根 / 换根。注意 `SetRoot` 不是挂接：`SetRootComponent(GetRootComponent())` 是空操作；
- **`GetCapsuleComponent()->InitCapsuleSize(半径, 半高)`**——定角色碰撞体尺寸；
- **`FindComponentByClass<T>()`**（AActor 模板函数）——在 Actor 的组件树里按类取**第一个**匹配（含子类），没有返回 nullptr。留给"组件不是自己创建、指针不在手里"的场景——自己构造期创建的组件攥着指针走 getter 零查找，按类查找是到**别人身上**找东西的唯一通道。

## 移动与朝向

- **SpringArm 三参数**——`TargetArmLength`（臂长）、`SocketOffset`（挂点偏移，越肩 = 右+上）、`bUsePawnControlRotation`（臂跟视角转；相机侧要关，双开转两倍）；配套 **`USpringArmComponent::SocketName`**——臂末端 Socket 的惯例名，相机挂它则臂伸多长相机飘多远；
- **`bUseControllerRotationPitch/Roll/Yaw`**（Pawn）——角色是否被控制器旋转锁死；走"移动朝向"方案三个必须显式 false（默认 Yaw 为 true）；
- **`GetCharacterMovement()`** 及四参数——取移动组件：`bOrientRotationToMovement`（朝移动方向转身）、`RotationRate`（只留 Yaw）、`MaxWalkSpeed`、`BrakingDecelerationWalking`（恒定刹车项；摩擦项 = `GroundFriction × BrakingFrictionFactor × 速度`，两者互补成完整刹车曲线）；
- **`AddMovementInput(方向, 强度)`**（APawn）——累积移动方向请求，实际位移由 CharacterMovementComponent 按速度/加速度结算。

## 调试输出

- **`GEngine->AddOnScreenDebugMessage(Key, Time, Color, Msg)`**——屏幕调试消息。`Key = -1` 每次追加新消息（记事件）；`Key ≥ 0` 固定值原地替换（盯状态）。打包后玩家同样可见，只作调试不作 UI。`GEngine` 是全局指针，调用前判空；
- **`UE_LOG(类别, 级别, 格式, ...)`**——输出日志（Output Log 主战场），带时间戳、可搜索、打包后可看文件。变参宏，`FString` 传 `%s` 要解引用。

## 标签与数据资产

- **`FGameplayTag`**——层级化 FName 标识符，全局注册表管理；`==` 精确比较（FName 级廉价），`MatchesTag` 父匹配（父标签匹配子标签）；
- **`FGameplayTag::RequestGameplayTag(FName)`**——编辑器路线按名取标签，拼错返回空标签静默失效——核心标签走 Native 常量；
- **`UDataAsset`**——"纯数据资产"基类：不存在世界里、可序列化的数据袋子；右键 Miscellaneous → Data Asset 建实例。"行为进 C++、配置进资产"的载体；
- **`FGameplayTag::IsValid()`**——标签是否非空：未设置、`RequestGameplayTag` 拼错返回的都是空标签。"当前装备武器"这类可空字段判空就靠它——nullptr 是合法状态时只判空返回，不 assert；
- **`HasTagExact` / `HasTag`**（FGameplayTagContainer）——前者只认一字不差的精确匹配；后者允许层级匹配，容器里的**父标签**会匹配子级查询。输入 tag 反查 Spec 用精确版：tag 是逐字配对的名牌不是层级分类，父级误伤（Spec 上挂了个光秃秃的父标签）要在这一行杜绝。

## 输入管线

- **`SetupPlayerInputComponent(UInputComponent*)`**——Pawn 被 Possess、InputComponent 就绪时由引擎回调；重生换角自动重走，绑定点放这里不会"绑在旧角色上蒸发"；
- **`GetController<APlayerController>()` / `GetLocalPlayer()`**——Character 侧模板取控制器，再取本地玩家；
- **`ULocalPlayer::GetSubsystem<T>(LocalPlayer)`**——按类型取本地玩家子系统的通用姿势；
- **`AddMappingContext(IMC, 优先级)`**（EnhancedInputLocalPlayerSubsystem）——挂输入映射上下文的标准路径；传 `nullptr` 静默无效；
- **`BindAction(IA, TriggerEvent, 对象, 函数)`**（UEnhancedInputComponent）——输入绑定的正式接口；工程封装成了模板糖 `BindNativeInputAction`；
- **`ETriggerEvent::Triggered`**——"按住期间持续触发"，移动/视角的常用档；
- **`ETriggerEvent::Started` / `ETriggerEvent::Completed`**——按下瞬间、松开瞬间各触发一次的"两拍"档；按键式能力的标准配置，套 `Triggered` 会变成按住期间每帧请求激活；
- **`BindAction` 的 payload 传参**——第 5 参起的实参原样透传进回调的参数表（可多个）；工程里把 InputTag 当 payload 传，统一的按下/松开回调靠参数分流所有能力输入，不必每个能力各写一个函数；
- **`FInputActionValue::Get<T>()`**——模板取回 IA 的值（`Get<FVector2D>()`）；IA 配的值类型和 T 对不上时拿到零值，不动但不崩；
- **`AddControllerYawInput / AddControllerPitchInput`**（APawn）——视角输入的转发口，喂给 PlayerController 的 ControlRotation；与 SpringArm 的 `bUsePawnControlRotation` 会师。

## 动画实例

- **`TryGetPawnOwner()`**（UAnimInstance）——AnimInstance 回溯主人（mesh → Pawn 两级一手包办），返回 `APawn*`；
- **`NativeInitializeAnimation()` / `NativeUpdateAnimation(float)`**（UAnimInstance 虚函数）——初始化（缓存主人）与逐帧更新（喂 GroundSpeed 等数据）的官方入口；重写都要先调 Super、逐帧入口要双判空；
- **`GetVelocity().Size2D()`**——速度水平模长（丢 Z 分量，起跳下落不搅乱 locomotion 轴）；
- **`GetCurrentAcceleration().SizeSquared2D() > 0`**——本帧是否有输入加速度；判零一律用平方版，省一次开方。

## GAS 接入与能力生命周期

- **`PossessedBy(AController*)`**（APawn 虚函数）——每次被控制器接管都触发（对比 BeginPlay 一生一次）；GAS 双端初始化约定的服务端半边，`InitAbilityActorInfo` 的标准时机；
- **`InitAbilityActorInfo(OwnerActor, AvatarActor)`**（ASC）——Owner = 逻辑/权威方，Avatar = 物理肉体；单机最简形态 `(this, this)`，AI/网络角色再分离；
- **`GetAvatarActor()`**（ASC）——ASC 侧取肉身 Actor，授予时填 `SourceObject` 用的就是它；GA 侧等价物是 `GetAvatarActorFromActorInfo()`（走 ActorInfo 缓存）；
- **`GetAbilitySystemComponent()`**（IAbilitySystemInterface 约定）——外部系统问 Actor 要 ASC 的统一入口，通用获取函数靠它工作；
- **`GetActorLabel()`**——编辑器构建专属（依赖 UnrealEd），Shipping 编不过；打包环境的打印用 `GetName()`；
- **`OnGiveAbility(ActorInfo, Spec)`**（UGameplayAbility 虚函数）——`GiveAbility` 落库时的回调，"授予的落点通知"；激活策略分岔在这里；
- **`EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled)`**（UGameplayAbility 虚函数）——结束回调；蓝图侧硬约定"逻辑走完必须调"，不调则 spec 永远处于激活态；
- **`TryActivateAbility(SpecHandle)`**（ASC）——请求激活能力；
- **`GetActivatableAbilities()`**（ASC）——已授予 Spec 容器的 const 引用（内部就是 AbilitySpecs 数组），ASC 输入侧反查的遍历源：逐份检查 `DynamicAbilityTags.HasTagExact(输入tag)`，命中拿 `Spec.Handle` 去 `TryActivateAbility`。数量级是"角色几十个能力"，线性扫足够，不值得建 tag→Spec 缓存表；
- **`ClearAbility(SpecHandle)`**（ASC）——把能力从 ASC 移除，用后即焚只该对 OnGiven 策略用；
- **`Spec.IsActive()`**——即 `ActiveCount > 0`；授予瞬间恒为 false（"OnGiven 自动激活"那个取反 bug 的案发现场）；
- **`GiveAbility(FGameplayAbilitySpec)`**（ASC）——正式授予接口；要求权威端调用（内部有所有权断言）；内部走到 `OnGiveAbility`；
- **`FGameplayAbilitySpec`**——能力在系统里的"运行时凭证"：构造时填类，`SourceObject` 溯源（装备/Buff/地形的通用槽位）、`Level` 缩放口（cost/cooldown/效果可随等级变）；
- **`DynamicAbilityTags`**（FGameplayAbilitySpec 成员）——挂在"这一次授予凭证"上的标签容器，区别于 GA 类上的 AbilityTags（类固有、全体实例共享）：同一个 GA 类可带不同 tag 授予多份互不干扰。授予时 `AddTag(输入tag)` 写入，ASC 输入侧按"Spec 的 DynamicAbilityTags 含某输入 tag"反查要激活的能力——tag 匹配是老 InputID 机制的现行替代；
- **`GetAvatarActorFromActorInfo()`**（UGameplayAbility）——取 ActorInfo 缓存的 Avatar Actor（蓝图节点 Get Avatar Actor from Actor Info 的 C++ 面）。Avatar ≠ Owner：Owner 是逻辑归属（AI 场景下可能是 Controller），Avatar 是场上物理执行能力的实体。GA 里找"能力作用于谁身上的东西"，一律从它出发；
- **`CurrentActorInfo`**（UGameplayAbility 成员）——`FGameplayAbilityActorInfo*`，能力激活/授予时由 ASC 灌入的运行时信息包，getter 家族（GetAvatarActorFromActorInfo 等）包装的就是它。三个成员常被直取：`AvatarActor`（肉体 Actor）、`PlayerController`（InitAbilityActorInfo 时已缓存，比沿 Avatar → GetController 链找省一步）、`AbilitySystemComponent`（**TWeakObjectPtr 弱引用**，用前 IsValid）。直取成员省一层函数调用，是 GA 里取 Actor/Controller/ASC 的快捷口。

## 武器与生成挂接

- **`SetBoxExtent(FVector)`**（UBoxComponent）——碰撞盒半边长；判定余量是手调值不是量出来的；
- **`SetCollisionEnabled(ECollisionEnabled::NoCollision)`**——碰撞开关；武器碰撞纪律"常闭、判定窗口显式开"；
- **`Spawn Actor from Class`**——`Collision Handling Override = Adjust If Possible But Always Spawn`："必须存在"优先于"出生点完美"；Owner / Instigator 都填 Avatar Pawn——Instigator 是"这把武器造成的事算谁的"归属凭据；
- **`Attach Actor To Component`**（C++ 真名 `AActor::AttachToComponent`）——挂 Socket 的三规则：Location = Snap To Target（贴齐 Socket，出生变换只活一毫秒）、Rotation = Keep Relative、Scale = Keep World（隔离父级缩放）；与 SceneComponent 上的同名函数（Attach Component To Component）是两个东西，查文档易串行；
- **`Get Skeletal Mesh Component from Actor Info`**（C++ 真名 `GetOwningComponentFromActorInfo`）——ActorInfo 缓存的骨骼网格组件；挂 Socket 必须以它为 Parent（Socket 是骨骼附属物），挂根组件连影子都摸不到；
- **`Event ActivateAbility`**（C++ 侧 `K2_ActivateAbility`）——能力激活的蓝图入口事件。

---

后续笔记遇到新 API，收进本页对应分组。
