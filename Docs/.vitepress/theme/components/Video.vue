<script setup lang="ts">
import { withBase } from 'vitepress'

// 用法（在 markdown 里）：
//   <Video src="/5.1动画节点无效配置.mp4" />
// src 必须是 public 目录下文件的根绝对路径（以 / 开头）。
// 用 withBase 包裹，自动适配部署的 base 路径：
//   本地 dev (base /)        → /5.1动画节点无效配置.mp4
//   GitHub Pages (base /ue5/) → /ue5/5.1动画节点无效配置.mp4
defineProps<{ src: string }>()
</script>

<template>
  <video :src="withBase(src)" controls width="100%">您的浏览器不支持视频播放</video>
</template>
