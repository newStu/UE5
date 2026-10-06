import type { Theme } from 'vitepress'
import DefaultTheme from 'vitepress/theme'
import { onMounted, watch, nextTick } from 'vue'
import { useRoute, useData } from 'vitepress'
import mediumZoom, { type Zoom } from 'medium-zoom'
import Video from './components/Video.vue'
import Layout from './Layout.vue'
import './styles.css'

// 保存 medium-zoom 实例，路由切换时先 detach 再重新 attach，
// 避免同一张图被多次绑定导致点击放大触发多次。
let zoom: Zoom | null = null

const initZoom = () => {
  zoom?.detach()
  // 只对「文档正文区」的图片生效（.vp-doc），不波及侧边栏 / 导航 / 首页。
  // background 用 VitePress 的背景色变量，遮罩自动跟随深浅色主题。
  zoom = mediumZoom('.vp-doc img', {
    background: 'var(--vp-c-bg)',
    margin: 24,
  })
}

// ---------- Mermaid 图表浏览器端渲染 ----------
// 实际的「加载 mermaid + 渲染」管线全在 public/mermaid-loader.js（经典 script，
// 经 config head 静态引入）——不放进本模块的原因见该文件头注释：主题 ESM 模块
// 上下文里动态加载脚本的回调会被浏览器搁置，经典上下文一切正常。
// 这里只保留调用入口。

declare global {
  interface Window {
    /** mermaid-loader.js 暴露的渲染入口：转换并渲染页面上所有 mermaid 代码块 */
    __mmRender?: (isDark: boolean, force?: boolean) => void
  }
}

// ---------- 全屏阅读模式 ----------
// 按钮与状态在 Layout.vue（顶栏插槽 + 右下角浮动按钮）和 fullscreen.ts 里，
// 这里不参与。

export default {
  extends: DefaultTheme,
  // 包一层默认主题 Layout：往顶栏插槽塞全屏按钮（见 Layout.vue）。
  // 注：Theme.setup() 由 VitePress 在 app 层调用，覆盖 Layout 不影响下面的 setup。
  Layout,
  // 全局注册 <Video> 组件，markdown 里可直接写 <Video src="/xxx.mp4" />。
  enhanceApp({ app }) {
    app.component('Video', Video)
  },
  setup() {
    const route = useRoute()
    const { isDark } = useData()

    const refresh = (forceMermaid = false) =>
      nextTick(() => {
        initZoom()
        window.__mmRender?.(isDark.value, forceMermaid)
      })

    // VitePress 的正文是异步挂载的（初次水合 / 代码组切换 tab 都会晚于 Layout 的
    // onMounted 出现），只靠 onMounted + 路由 watch 会错过内容出现的时机 ——
    // 用 MutationObserver 兜底：只要页面上冒出未转换的 mermaid 代码块就补渲染。
    let observerTimer: ReturnType<typeof setTimeout> | undefined

    onMounted(() => {
      refresh()
      new MutationObserver(() => {
        if (!document.querySelector('div.language-mermaid')) return
        clearTimeout(observerTimer)
        observerTimer = setTimeout(() => window.__mmRender?.(isDark.value), 100)
      }).observe(document.body, { childList: true, subtree: true })
    })

    // 切到新页面后，DOM 重建，需要重新绑定新页面的图片、渲染新页面的图表。
    watch(() => route.path, () => refresh())
    // 深浅色切换后用对应主题强制重渲染图表。
    watch(isDark, () => refresh(true))
  },
} satisfies Theme
