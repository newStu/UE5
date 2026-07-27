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
        ],
      },
    ],

    // 右上角仓库入口（指向 Gitee 主仓库，用 Gitee 风格的红色 G 图标）。
    socialLinks: [
      {
        icon: {
          svg: '<svg viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><circle cx="12" cy="12" r="11" fill="#C71D23"/><text x="12" y="17" font-size="14" font-weight="700" fill="#fff" text-anchor="middle" font-family="Arial, sans-serif">G</text></svg>',
        },
        link: 'https://gitee.com/wangzhaoyv/ue5',
        ariaLabel: 'Gitee 仓库',
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
