import type { Theme } from 'vitepress'
import DefaultTheme from 'vitepress/theme'
import { onMounted, watch, nextTick } from 'vue'
import { useRoute, useData } from 'vitepress'
import mediumZoom, { type Zoom } from 'medium-zoom'
import Video from './components/Video.vue'
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

// ---------- Mermaid 图表浏览器端渲染（自托管，同 medium-zoom 的接入方式） ----------
// ```mermaid 代码块在构建期不做任何处理（mermaid 不进打包流程，避免构建期风险），
// 页面加载后按需请求 /mermaid.min.js（Docs/public/ 下的官方单文件产物），
// 把 code.language-mermaid 替换为渲染出的 SVG。渲染失败时保留源码，方便排查语法错误。

declare global {
  interface Window {
    mermaid?: {
      initialize: (config: Record<string, unknown>) => void
      render: (id: string, text: string) => Promise<{ svg: string }>
    }
  }
}

let mermaidLoading: Promise<void> | null = null

const loadMermaid = (base: string): Promise<void> => {
  if (window.mermaid) return Promise.resolve()
  if (!mermaidLoading) {
    mermaidLoading = new Promise((resolve, reject) => {
      const script = document.createElement('script')
      script.src = `${base}mermaid.min.js`
      script.onload = () => resolve()
      script.onerror = () => {
        mermaidLoading = null // 允许下次（如路由切换后）重试
        reject(new Error(`加载 ${base}mermaid.min.js 失败`))
      }
      document.head.appendChild(script)
    })
  }
  return mermaidLoading
}

// 转义源码，渲染失败时按纯文本展示用。
const escapeHtml = (text: string) =>
  text.replace(/[<>&]/g, (c) => ({ '<': '&lt;', '>': '&gt;', '&': '&amp;' })[c] as string)

const renderMermaid = async (base: string, isDark: boolean) => {
  const codeBlocks = document.querySelectorAll<HTMLElement>('code.language-mermaid')
  const boxes = document.querySelectorAll<HTMLElement>('.mermaid-box')
  if (!codeBlocks.length && !boxes.length) return

  try {
    await loadMermaid(base)
  } catch (err) {
    console.warn('[mermaid]', err)
    return
  }

  const mermaid = window.mermaid!
  mermaid.initialize({ startOnLoad: false, theme: isDark ? 'dark' : 'default' })

  // 首次渲染：把 ```mermaid 代码块整个换成容器，源码存进 data 属性（供主题切换时重渲染）。
  for (const code of Array.from(codeBlocks)) {
    const box = document.createElement('div')
    box.className = 'mermaid-box'
    box.dataset.mermaidSrc = code.textContent ?? ''
    ;(code.closest('div[class*="language-mermaid"]') ?? code).replaceWith(box)
  }

  // 容器统一渲染（含主题切换后已存在的 .mermaid-box）。
  for (const box of Array.from(document.querySelectorAll<HTMLElement>('.mermaid-box'))) {
    const src = box.dataset.mermaidSrc ?? ''
    const id = `mmd-${Math.random().toString(36).slice(2, 10)}`
    try {
      const { svg } = await mermaid.render(id, src)
      box.innerHTML = svg
    } catch (err) {
      console.warn('[mermaid] 渲染失败：', err)
      box.innerHTML = `<pre class="mermaid-fallback">${escapeHtml(src)}</pre>`
    }
  }
}

export default {
  extends: DefaultTheme,
  // 全局注册 <Video> 组件，markdown 里可直接写 <Video src="/xxx.mp4" />。
  enhanceApp({ app }) {
    app.component('Video', Video)
  },
  setup() {
    const route = useRoute()
    const { site, isDark } = useData()
    const refresh = () => nextTick(() => {
      initZoom()
      renderMermaid(site.value.base, isDark.value)
    })

    onMounted(refresh)
    // 切到新页面后，DOM 重建，需要重新绑定新页面的图片、渲染新页面的图表。
    watch(() => route.path, refresh)
    // 深浅色切换后用对应主题重渲染图表。
    watch(isDark, refresh)
  },
} satisfies Theme
