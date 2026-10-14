---
layout: home

hero:
  name: UE5 学习笔记
  text: 蓝图入门 · C++ 进阶
  image:
    src: /hero.svg
    alt: UE5 学习笔记
  tagline: 跟随 B 站教程的双线实操记录 —— 蓝图篇从角色移动到场景机关与完整游戏流程，C++ 篇从组件、射线检测到接口、UMG 与音效。
  actions:
    - theme: brand
      text: 蓝图入门
      link: /蓝图/1.创建角色移动/创建角色移动
    - theme: brand
      text: C++ 进阶
      link: /C++/
    - theme: alt
      text: GitHub 仓库
      link: https://github.com/newStu/UE5

features:
  - icon: 🏃
    title: 1. 创建角色移动
    details: 自定义 GameMode / Character，绑定增强输入系统（IA / IMC），实现 WASD 移动 + 鼠标视角。
    link: /蓝图/1.创建角色移动/创建角色移动
  - icon: 🎞️
    title: 2. 动画系统
    details: AnimBP + Blend Space，按角色水平速度在站立 / 奔跑之间自动混合。
    link: /蓝图/2.动画创建/动画创建
  - icon: 🎥
    title: 3. 第三人称视角
    details: SpringArm + Camera 挂载，基于相机朝向的移动与视角控制。
    link: /蓝图/3.第三人称角色移动/第三人称角色移动
  - icon: 🦘
    title: 4. 跳跃状态机
    details: Jump_Start / Jump_Loop / Jump_End 三态切换，由 Z 轴速度驱动。
    link: /蓝图/4.角色跳跃/角色跳跃
  - icon: 🎪
    title: 5. 场景机关
    details: 大摆锤、跷跷板、旋转指针、妙笔生花、蹦床、滚木六类机关搭建。
    link: /蓝图/5.制作场景地图/5.1制作大摆锤/制作大摆锤
  - icon: 🏁
    title: 6. 死亡 / 重生 / 胜利
    details: 碰撞触发死亡、检查点重生、胜利结算的完整游戏闭环流程。
    link: /蓝图/6.角色死亡设置/角色死亡设置
  - icon: 📡
    title: 10. 蓝图通讯
    details: 蓝图接口与事件分发器两种解耦方式——接口点名调用、分发器一对多广播。
    link: /蓝图/10.蓝图通讯/蓝图接口/蓝图接口
  - icon: 🧩
    title: C++ 组件与反射
    details: 增强输入绑定、自定义组件与生命周期、常用宏（UPROPERTY/UFUNCTION）与反射系统。
    link: /C++/3.组件编写与生命周期
  - icon: 🎯
    title: C++ 射线检测与碰撞
    details: LineTrace 视线检测、自定义碰撞通道与预设、子弹命中回调 OnComponentHit。
    link: /C++/13.LineTrace射线检测
  - icon: 💉
    title: C++ 战斗闭环
    details: 接口解耦命中判定、血量组件、UMG 血条、HUD 管理、音效与命中特效。
    link: /C++/19.实战-接口Interface与命中解耦
---

> 📺 **编写说明**：蓝图部分跟随 B 站视频教程 [《【最新版 5.7】UE5 零基础入门教程》](https://www.bilibili.com/video/BV1qYSvBHELW) 学习整理而成；C++ 部分跟随 [UE5 C++ 实战教程](https://www.bilibili.com/video/BV1af421R7BD/) 学习整理而成。均按章节逐步实操记录，感谢 UP 主的优质教程。
