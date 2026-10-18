import { defineConfig } from 'vitepress'

// base 从环境变量读取（GitHub Actions 里按仓库名自动算出 <user>.github.io → '/', 其他 → '/<repo>/'）。
// 本地 `npm run docs:dev` 时为 undefined，回退到 '/'。
const base = process.env.BASE_PATH || '/'

// ---------------- 侧边栏数据 ----------------
// 按路径前缀分流：/C++/ 页面只显示 C++ 目录，/RPG/ 页面只显示 RPG 目录，
// /蓝图/ 页面只显示蓝图目录，/编辑器/ 页面只显示编辑器目录；其余页面（如首页）兜底显示全部分类。

const cppSidebar = {
  text: 'C++',
  collapsed: false,
  items: [
    { text: '简介', link: '/C++/' },
    { text: '1. Character 与 GameMode', link: '/C++/1.Character与GameMode的C++实现' },
    { text: '2. 增强输入系统', link: '/C++/2.增强输入系统的C++绑定' },
    { text: '3. 组件编写与生命周期', link: '/C++/3.组件编写与生命周期' },
    { text: '4. 常用宏与反射', link: '/C++/4.常用宏与反射' },
    { text: '5. 输入控制与第三人称相机', link: '/C++/5.输入控制与第三人称相机' },
    { text: '6. 动画类与蓝图状态机绑定', link: '/C++/6.动画类的C++实现与蓝图状态机绑定' },
    { text: '7. 拓展：为跳跃状态机暴露变量', link: '/C++/7.拓展-为跳跃状态机暴露C++变量' },
    { text: '8. 面向对象与 Actor/Pawn/Character', link: '/C++/8.面向对象与Actor-Pawn-Character' },
    { text: '9. 运行顺序与生命周期（常用篇）', link: '/C++/9.运行顺序与生命周期-常用篇' },
    { text: '10. 查阅：Actor 与组件生命周期', link: '/C++/10.查阅-Actor与组件生命周期全表' },
    { text: '11. 查阅：引擎启动与框架类生命周期', link: '/C++/11.查阅-引擎启动与框架类生命周期' },
    { text: '12. TArray 动态数组', link: '/C++/12.TArray动态数组' },
    { text: '13. LineTrace 射线检测', link: '/C++/13.LineTrace射线检测' },
    { text: '14. 实战：把视线检测重构为组件', link: '/C++/14.实战-把视线检测重构为组件' },
    { text: '15. 查阅：引擎常用组件一览', link: '/C++/15.查阅-引擎常用组件一览' },
    { text: '16. 实战：发射子弹与定时开火', link: '/C++/16.实战-发射子弹与定时开火' },
    { text: '17. 自定义碰撞通道与碰撞预设', link: '/C++/17.自定义碰撞通道与碰撞预设' },
    { text: '18. 实战：子弹命中回调 OnComponentHit', link: '/C++/18.实战-子弹命中回调OnComponentHit' },
    { text: '19. 实战：接口 Interface 与命中解耦', link: '/C++/19.实战-接口Interface与命中解耦' },
    { text: '20. 实战：血量组件 HealthComponent', link: '/C++/20.实战-血量组件HealthComponent' },
    { text: '21. 实战：UMG 血条与死亡重开界面', link: '/C++/21.实战-UMG血条与死亡重开界面' },
    { text: '22. 实战：HUD 类与血条管理重构', link: '/C++/22.实战-HUD类与血条管理重构' },
    { text: '23. 实战：音效与命中特效', link: '/C++/23.实战-音效与命中特效' },
  ],
}

// 独立的顶级栏目：RPG 游戏开发（Warrior 工程），与 C++ 主线并列，编号从 1 重新起。
const warriorSidebar = {
  text: 'RPG',
  collapsed: false,
  items: [
    { text: '1. RPG 项目骨架与 Base/Hero 类分层', link: '/RPG/1.项目骨架与Base-Hero类分层' },
    { text: '2. 调试打印与 Debug 助手', link: '/RPG/2.调试打印与Debug助手' },
    { text: '3. 相机臂与移动朝向配置', link: '/RPG/3.相机臂与移动朝向配置' },
    { text: '4. Native Gameplay Tags', link: '/RPG/4.NativeGameplayTags' },
    { text: '5. 输入绑定与 InputConfig 数据资产', link: '/RPG/5.输入绑定与InputConfig数据资产' },
    { text: '6. 自定义 WarriorInputComponent', link: '/RPG/6.自定义WarriorInputComponent' },
  ],
}

const bpSidebar = {
  text: '蓝图',
  collapsed: false,
  items: [
    { text: '1. 创建角色移动', link: '/蓝图/1.创建角色移动/创建角色移动' },
    { text: '2. 动画创建', link: '/蓝图/2.动画创建/动画创建' },
    { text: '3. 第三人称角色移动', link: '/蓝图/3.第三人称角色移动/第三人称角色移动' },
    { text: '4. 角色跳跃', link: '/蓝图/4.角色跳跃/角色跳跃' },
    {
      text: '5. 制作场景地图',
      collapsed: false,
      items: [
        { text: '5.1 大摆锤', link: '/蓝图/5.制作场景地图/5.1制作大摆锤/制作大摆锤' },
        { text: '5.2 时机跳跷跷障碍', link: '/蓝图/5.制作场景地图/5.2时机跳跷跷障碍/时机跳跷跷障碍' },
        { text: '5.3 旋转指针', link: '/蓝图/5.制作场景地图/5.3旋转指针/旋转指针' },
        { text: '5.4 妙笔生花', link: '/蓝图/5.制作场景地图/5.4妙笔生花/妙笔生花' },
        { text: '5.5 蹦床', link: '/蓝图/5.制作场景地图/5.5蹦床/蹦床' },
        { text: '5.6 滚木', link: '/蓝图/5.制作场景地图/5.6滚木/滚木' },
      ],
    },
    { text: '6. 角色死亡设置', link: '/蓝图/6.角色死亡设置/角色死亡设置' },
    { text: '7. 死亡后重生', link: '/蓝图/7.死亡后重生/死亡后重生' },
    { text: '8. 检查点制作', link: '/蓝图/8.检查点制作/检查点制作' },
    { text: '9. 游戏胜利', link: '/蓝图/9.游戏胜利/游戏胜利' },
    {
      text: '10. 蓝图通讯',
      collapsed: false,
      items: [
        { text: '10.1 蓝图接口', link: '/蓝图/10.蓝图通讯/蓝图接口/蓝图接口' },
        { text: '10.2 事件分发器', link: '/蓝图/10.蓝图通讯/事件分发器/事件分发器' },
      ],
    },
    { text: '特殊节点', link: '/蓝图/特殊节点/特殊节点' },
    { text: '数学知识', link: '/蓝图/数学知识/数学知识' },
  ],
}

const editorSidebar = {
  text: '编辑器',
  collapsed: false,
  items: [
    { text: 'UE5 编辑器', link: '/编辑器/UE5编辑器/UE5编辑器' },
    { text: 'UE5 建模模式', link: '/编辑器/UE5建模模式/UE5建模模式' },
    { text: '材质编辑', link: '/编辑器/材质编辑/' },
  ],
}

export default defineConfig({
  base,
  lang: 'zh-CN',
  title: 'UE5 学习/演示工程文档',
  description:
    'Unreal Engine 学习工程 —— 蓝图入门与 C++ 进阶双线实操笔记：角色移动、动画、场景机关，以及组件、接口、UMG、音效等 C++ 玩法实现',
  lastUpdated: true,
  // 不启用 cleanUrls：GitHub Pages 对无后缀 URL 支持不稳，带 .html 最保险。

  // Mermaid 浏览器端渲染器（经典 script，public/ 下的静态文件，随站点根部署）。
  // 必须走 head 静态引入而不是主题模块动态注入：见 mermaid-loader.js 文件头注释。
  // 注：VitePress 不会给 head 里的 src 自动加 base 前缀，需手动拼。
  head: [['script', { src: `${base}mermaid-loader.js`, defer: '' }]],

  themeConfig: {
    siteTitle: 'UE5 文档',

    // 搜索：用 VitePress 内置的本地搜索，无需第三方服务/Key。
    search: {
      provider: 'local',
      options: {
        translations: {
          button: {
            buttonText: '搜索文档',
            buttonAriaLabel: '搜索文档',
          },
          modal: {
            noResultsText: '无法找到相关结果',
            resetButtonTitle: '清除查询条件',
            footer: {
              selectText: '选择',
              navigateText: '切换',
              closeText: '关闭',
            },
          },
        },
      },
    },

    // 顶部导航：C++ 与蓝图是两条独立学习线，各占一个顶级入口；
    // 特殊节点/数学知识等蓝图参考已收进蓝图侧边栏，编辑器保留下拉。
    nav: [
      { text: '首页', link: '/' },
      { text: 'C++', link: '/C++/' },
      { text: 'RPG', link: '/RPG/1.项目骨架与Base-Hero类分层' },
      { text: '蓝图', link: '/蓝图/1.创建角色移动/创建角色移动' },
      {
        text: '编辑器',
        items: [
          { text: 'UE5 编辑器', link: '/编辑器/UE5编辑器/UE5编辑器' },
          { text: 'UE5 建模模式', link: '/编辑器/UE5建模模式/UE5建模模式' },
          { text: '材质编辑', link: '/编辑器/材质编辑/' },
        ],
      },
    ],

    // 侧边栏：按路径前缀分流 —— /C++/ 只显示 C++ 目录，/RPG/ 只显示
    // RPG 目录，/蓝图/ 只显示蓝图目录，/编辑器/ 只显示编辑器目录，
    // 其余页面（首页）兜底显示全部分类。
    sidebar: {
      '/C++/': [cppSidebar],
      '/RPG/': [warriorSidebar],
      '/蓝图/': [bpSidebar],
      '/编辑器/': [editorSidebar],
      '/': [cppSidebar, warriorSidebar, bpSidebar, editorSidebar],
    },

    // 右上角仓库入口（指向 GitHub 主仓库，用 GitHub 图标）。
    socialLinks: [
      {
        icon: {
          svg: '<svg viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path d="M12 .297c-6.63 0-12 5.373-12 12 0 5.303 3.438 9.8 8.205 11.385.6.113.82-.258.82-.577 0-.285-.01-1.04-.015-2.04-3.338.724-4.042-1.61-4.042-1.61C4.422 18.07 3.633 17.7 3.633 17.7c-1.087-.744.084-.729.084-.729 1.205.084 1.838 1.236 1.838 1.236 1.07 1.835 2.809 1.305 3.495.998.108-.776.417-1.305.76-1.605-2.665-.3-5.466-1.332-5.466-5.93 0-1.31.465-2.38 1.235-3.22-.135-.303-.54-1.523.105-3.176 0 0 1.005-.322 3.3 1.23.96-.267 1.98-.399 3-.405 1.02.006 2.04.138 3 .405 2.28-1.552 3.285-1.23 3.285-1.23.645 1.653.24 2.873.12 3.176.765.84 1.23 1.91 1.23 3.22 0 4.61-2.805 5.625-5.475 5.92.42.36.81 1.096.81 2.22 0 1.606-.015 2.896-.015 3.286 0 .315.21.69.825.57C20.565 22.092 24 17.592 24 12.297c0-6.627-5.373-12-12-12"/></svg>',
        },
        link: 'https://github.com/newStu/UE5',
        ariaLabel: 'GitHub 仓库',
      },
    ],

    // 中文化 UI 文案。
    outline: { label: '本页目录', level: [2, 3] },
    docFooter: { prev: '下一篇', next: '下一篇' },
    lastUpdated: { text: '最后更新于' },
    returnToTopLabel: '回到顶部',
    sidebarMenuLabel: '目录',
    darkModeSwitchLabel: '主题',
    lightModeSwitchTitle: '切换到浅色模式',
    darkModeSwitchTitle: '切换到深色模式',

    footer: {
      message: '基于 Unreal Engine · 蓝图与 C++ 学习工程',
      copyright: 'MIT License',
    },
  },
})
