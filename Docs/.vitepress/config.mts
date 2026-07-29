import { defineConfig } from 'vitepress'

// base 从环境变量读取（GitHub Actions 里按仓库名自动算出 <user>.github.io → '/', 其他 → '/<repo>/'）。
// 本地 `npm run docs:dev` 时为 undefined，回退到 '/'。
export default defineConfig({
  base: process.env.BASE_PATH || '/',
  lang: 'zh-CN',
  title: 'UE5 学习/演示工程文档',
  description:
    'Unreal Engine 5.8 纯蓝图工程 —— 角色移动、动画、跳跃、场景机关、死亡重生与胜利流程的实操笔记',
  lastUpdated: true,
  // 不启用 cleanUrls：GitHub Pages 对无后缀 URL 支持不稳，带 .html 最保险。

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

    // 顶部导航：保持线性，不按主题分类，引导从头开始顺序学习。
    nav: [
      { text: '首页', link: '/' },
      { text: '开始学习', link: '/1.创建角色移动/创建角色移动' },
      {
        text: '参考资料',
        items: [
          { text: 'UE5 编辑器', link: '/UE5编辑器/UE5编辑器' },
          { text: '特殊节点', link: '/特殊节点/特殊节点' },
          { text: '数学知识', link: '/数学知识/数学知识' },
        ],
      },
    ],

    // 侧边栏：严格按 Docs/ 目录的编号顺序平铺，忠实保留学习路径（不按主题重新分组）。
    sidebar: [
      { text: '1. 创建角色移动', link: '/1.创建角色移动/创建角色移动' },
      { text: '2. 动画创建', link: '/2.动画创建/动画创建' },
      { text: '3. 第三人称角色移动', link: '/3.第三人称角色移动/第三人称角色移动' },
      { text: '4. 角色跳跃', link: '/4.角色跳跃/角色跳跃' },
      {
        text: '5. 制作场景地图',
        collapsed: false,
        items: [
          { text: '5.1 大摆锤', link: '/5.制作场景地图/5.1制作大摆锤/制作大摆锤' },
          { text: '5.2 时机跳跷跷障碍', link: '/5.制作场景地图/5.2时机跳跷跷障碍/时机跳跷跷障碍' },
          { text: '5.3 旋转指针', link: '/5.制作场景地图/5.3旋转指针/旋转指针' },
          { text: '5.4 妙笔生花', link: '/5.制作场景地图/5.4妙笔生花/妙笔生花' },
          { text: '5.5 蹦床', link: '/5.制作场景地图/5.5蹦床/蹦床' },
          { text: '5.6 滚木', link: '/5.制作场景地图/5.6滚木/滚木' },
        ],
      },
      { text: '6. 角色死亡设置', link: '/6.角色死亡设置/角色死亡设置' },
      { text: '7. 死亡后重生', link: '/7.死亡后重生/死亡后重生' },
      { text: '8. 检查点制作', link: '/8.检查点制作/检查点制作' },
      { text: '9. 游戏胜利', link: '/9.游戏胜利/游戏胜利' },
      {
        text: '参考资料',
        collapsed: false,
        items: [
          { text: 'UE5 编辑器', link: '/UE5编辑器/UE5编辑器' },
          { text: '特殊节点', link: '/特殊节点/特殊节点' },
          { text: '数学知识', link: '/数学知识/数学知识' },
        ],
      },
    ],

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
    docFooter: { prev: '上一篇', next: '下一篇' },
    lastUpdated: { text: '最后更新于' },
    returnToTopLabel: '回到顶部',
    sidebarMenuLabel: '目录',
    darkModeSwitchLabel: '主题',
    lightModeSwitchTitle: '切换到浅色模式',
    darkModeSwitchTitle: '切换到深色模式',

    footer: {
      message: '基于 Unreal Engine 5.8 · 纯蓝图工程',
      copyright: 'MIT License',
    },
  },
})
