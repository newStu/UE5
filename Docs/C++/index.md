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

## 学习建议

- 每章代码都能在 `MyThird/Source/MyThird/` 里找到对应文件，建议边读边改边编译验证；
- 蓝图章节的成果（IA / IMC 资产、角色蓝图）在 C++ 版里继续复用——C++ 与蓝图是混用关系，不是替换；
- 遇到宏相关报错（`.generated.h`、`GENERATED_BODY`）先翻 [第 4 章](/C++/4.常用宏与反射.md)的"常见坑汇总"。
