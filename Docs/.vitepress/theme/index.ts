import type { Theme } from 'vitepress'
import DefaultTheme from 'vitepress/theme'
import { onMounted, watch, nextTick } from 'vue'
import { useRoute } from 'vitepress'
import mediumZoom, { type Zoom } from 'medium-zoom'
import Video from './components/Video.vue'
import './styles.css'

// 保存当前 medium-zoom 实例，路由切换时先 detach 再重新 attach，
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

export default {
  extends: DefaultTheme,
  // 全局注册 <Video> 组件，markdown 里可直接用 <Video src="/xxx.mp4" />。
  enhanceApp({ app }) {
    app.component('Video', Video)
  },
  setup() {
    const route = useRoute()
    onMounted(initZoom)
    // 切到新页面后，DOM 重建，需要重新绑定新页面的图片。
    watch(() => route.path, () => nextTick(initZoom))
  },
} satisfies Theme
