<script setup lang="ts">
import DefaultTheme from 'vitepress/theme'
import { onMounted } from 'vue'
import { isFullscreen, toggleFullscreen, setFullscreen, restoreFullscreen } from './fullscreen'

const { Layout } = DefaultTheme

const label = () => (isFullscreen.value ? '退出全屏阅读（Esc）' : '进入全屏阅读')

onMounted(() => {
  // Esc 退出全屏。
  document.addEventListener('keydown', (e) => {
    if (e.key === 'Escape') setFullscreen(false)
  })
  // 恢复上次会话的全屏状态。
  restoreFullscreen()
})
</script>

<template>
  <Layout>
    <!-- 桌面端：顶栏右侧（外观切换 / 社交图标旁）的全屏按钮 -->
    <template #nav-bar-content-after>
      <button
        class="fullscreen-nav-toggle"
        :title="label()"
        :aria-label="label()"
        @click="toggleFullscreen"
      >
        {{ isFullscreen ? '✕' : '⛶' }}
      </button>
    </template>

    <!-- 移动端：展开的导航抽屉底部 -->
    <template #nav-screen-content-after>
      <button
        class="fullscreen-nav-toggle"
        :title="label()"
        :aria-label="label()"
        @click="toggleFullscreen"
      >
        {{ isFullscreen ? '✕ 退出全屏' : '⛶ 全屏阅读' }}
      </button>
    </template>
  </Layout>

  <!-- 右下角浮动按钮：全屏后顶栏被隐藏，靠它（或 Esc）退出。 -->
  <button
    class="fullscreen-toggle"
    :title="label()"
    :aria-label="label()"
    @click="toggleFullscreen"
  >
    {{ isFullscreen ? '✕' : '⛶' }}
  </button>
</template>
