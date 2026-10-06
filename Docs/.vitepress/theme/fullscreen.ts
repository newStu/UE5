import { ref } from 'vue'

// ---------- 全屏阅读模式（共享状态） ----------
// 隐藏侧边栏 / 右侧大纲 / 顶部导航，正文占满全宽。
// 顶栏按钮与右下角浮动按钮（Layout.vue）共用同一份状态与切换逻辑。

const FS_KEY = 'ue5-docs-fullscreen'
const FS_CLASS = 'docs-fullscreen'

/** 响应式全屏状态，按钮图标 / title 跟随它变化。 */
export const isFullscreen = ref(false)

export const setFullscreen = (on: boolean) => {
  isFullscreen.value = on
  document.documentElement.classList.toggle(FS_CLASS, on)
  try {
    localStorage.setItem(FS_KEY, on ? '1' : '0')
  } catch {
    /* 隐私模式下 localStorage 可能不可用，忽略 */
  }
}

export const toggleFullscreen = () => setFullscreen(!isFullscreen.value)

/** 恢复上次会话的全屏状态（onMounted 时调用，避免 SSR 期访问 localStorage）。 */
export const restoreFullscreen = () => {
  try {
    if (localStorage.getItem(FS_KEY) === '1') setFullscreen(true)
  } catch {
    /* 同上 */
  }
}
