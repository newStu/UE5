# C++ 笔记

UE5 C++ 开发相关笔记，配合 `MyThird` C++ 工程整理——把前面蓝图章节做过的东西（角色移动、输入绑定、组件、死亡重生）逐个用 C++ 重新实现一遍，并补充蓝图视角看不到的底层机制。

## 目录

| 章节 | 内容 | 对照蓝图章节 |
|---|---|---|
| [1. Character 与 GameMode 的 C++ 实现](/C++/1.Character与GameMode的C++实现.md) | 新建 C++ 类、GameMode 指定默认 Pawn、构造函数配置组件与参数、生命周期入口对照 | 1. 创建角色移动 |
| [2. 增强输入系统的 C++ 绑定](/C++/2.增强输入系统的C++绑定.md) | IA / IMC 引用、AddMappingContext、BindAction 与 ETriggerEvent、Move / Look 实现 | 1. 创建角色移动（输入部分） |
| [3. 组件编写与生命周期](/C++/3.组件编写与生命周期.md) | 组件三基类、CreateDefaultSubobject、自定义组件、Actor 生命周期、组件委托（事件分发器） | 3. 第三人称角色移动、10. 蓝图通讯 |
| [4. 常用宏与反射](/C++/4.常用宏与反射.md) | UPROPERTY / UFUNCTION / UCLASS / USTRUCT / UENUM 说明符速查与常见坑 | —（C++ 基础设施） |
| [5. 输入控制与第三人称相机](/C++/5.输入控制与第三人称相机.md) | 手写 PlayerCharacter：SpringArm/相机组件、三步旋转配置、Move 两种写法、Look → 相机链路 | 3. 第三人称角色移动 |
| [6. 动画类的 C++ 实现与蓝图状态机绑定](/C++/6.动画类的C++实现与蓝图状态机绑定.md) | 手写 UPlayerAnim：缓存角色/移动组件、每帧算 Speed、蓝图状态机与混合空间消费 C++ 变量 | 2. 动画创建 |
| [7. 拓展：为跳跃状态机暴露变量](/C++/7.拓展-为跳跃状态机暴露C++变量.md) | IsFalling / VerticalVelocity 的暴露思路与跳跃转换条件（拓展前瞻，工程尚未绑定跳跃） | 4. 角色跳跃 |
| [8. 面向对象与 Actor/Pawn/Character](/C++/8.面向对象与Actor-Pawn-Character.md) | 封装/继承/多态/组合、Super 与虚函数、Actor → Pawn → Character 继承链与选型 | —（C++ 基础设施） |
| [9. 运行顺序与生命周期（常用篇）](/C++/9.运行顺序与生命周期-常用篇.md) | 启动到退出的总览流程图、SpawnActor 完整链、常用生命周期函数（ctor / PostInitializeComponents / BeginPlay / Tick / EndPlay / Destroyed）与坑 | 3. 组件编写与生命周期 |
| [10. 查阅：Actor 与组件生命周期全表](/C++/10.查阅-Actor与组件生命周期全表.md) | Actor 生成/初始化/运行/销毁四阶段全表、UActorComponent 全表、Actor 与组件相对顺序对照（查阅手册） | 3. 组件编写与生命周期 |
| [11. 查阅：引擎启动与框架类生命周期](/C++/11.查阅-引擎启动与框架类生命周期.md) | 引擎→世界启动时序、玩家加入时序图、GameMode / GameState / Controller / Pawn / GameInstance / 子系统生命周期表（查阅手册） | 1. Character 与 GameMode |
| [12. TArray 动态数组](/C++/12.TArray动态数组.md) | 创建/添加/移除/遍历/查找/排序、Num/Max/Slack 内存模型、RemoveAtSwap 技巧、GC 与 UPROPERTY、常见坑 | —（C++ 基础设施） |
| [13. LineTrace 射线检测](/C++/13.LineTrace射线检测.md) | 五要素、Trace 通道与自定义渠道、Sweep 对照、Single 与 Multi / Block 与 Overlap | —（C++ 基础设施） |
| [14. 实战：把视线检测重构为组件](/C++/14.实战-把视线检测重构为组件.md) | 把 EnemyCharacter 的视线检测下放为 UMySceneComponent：基类选择、目标注入、Actor↔组件 API 换算、挂载三步 | 3. 组件编写与生命周期 |
| [15. 查阅：引擎常用组件一览](/C++/15.查阅-引擎常用组件一览.md) | 形状/网格体/移动/相机/灯光/特效音频/工具/物理组件速查表、UPrimitiveComponent 碰撞与三事件、组件查找入口（查阅手册） | 3. 组件编写与生命周期 |
| [16. 实战：发射子弹与定时开火](/C++/16.实战-发射子弹与定时开火.md) | BallProjectile 组装（球体碰撞+投射物移动）、TSubclassOf 类引用、SpawnActorDeferred 延迟生成、边沿触发 + FTimerHandle 定时开火 | —（视线检测实战续章） |
| [17. 自定义碰撞通道与碰撞预设](/C++/17.自定义碰撞通道与碰撞预设.md) | 碰撞配置三层结构（通道/预设/组件）、新建 ObjectType 通道与自定义 Profile、SetCollisionProfileName、SetNotifyRigidBodyCollision 开启物理 Hit 事件 | —（第 13/16 章续章） |
| [18. 实战：子弹命中回调 OnComponentHit](/C++/18.实战-子弹命中回调OnComponentHit.md) | OnComponentHit 委托绑定（AddDynamic + UFUNCTION）、回调参数逐个看、Cast 判断撞到的是玩家还是墙、Destroy 一次性销毁 | —（第 16/17 章续章） |

## 学习建议

- 每章代码都能在 `MyThird/Source/MyThird/` 里找到对应文件，建议边读边改边编译验证；
- 蓝图章节的成果（IA / IMC 资产、角色蓝图）在 C++ 版里继续复用——C++ 与蓝图是混用关系，不是替换；
- 遇到宏相关报错（`.generated.h`、`GENERATED_BODY`）先翻 [第 4 章](/C++/4.常用宏与反射.md)的"常见坑汇总"。
