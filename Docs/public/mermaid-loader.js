/**
 * Mermaid 浏览器端渲染器（经典 script，由 VitePress head 静态引入，随 public/ 部署在站点根）。
 *
 * 为什么不放进主题 ESM 模块：实测在主题模块上下文里动态加载 mermaid
 * （script 标签的 onload 或 fetch 的 Promise 链）会被浏览器搁置、永远不回调；
 * 而同样的代码在经典 script 上下文一切正常。因此整条「加载 + 渲染」管线
 * 都放在本文件里，主题层只通过 window.__mmRender(isDark, force) 调用。
 *
 * ```mermaid 代码块在构建期不做任何处理（mermaid 不进打包流程），
 * 页面加载后按需 fetch 同目录的 mermaid.min.js（官方 UMD 单文件产物），
 * 把 div.language-mermaid（VitePress + Shiki 的实际包裹结构）替换为渲染出的 SVG。
 * 渲染失败时保留源码，方便排查语法错误。
 */
;(function () {
  if (window.__mmRender) return

  // 从本脚本自身的 URL 推导站点 base（本文件与 mermaid.min.js 同在站点根目录），
  // 本地 '/' 与 GitHub Pages '/UE5/' 都自动正确，不依赖调用方传参。
  var base = '/'
  try {
    var self = document.currentScript
    if (self && self.src) base = self.src.replace(/[^/]*$/, '')
  } catch (e) {
    /* 保持 '/' */
  }

  var loading = null

  function loadMermaid() {
    if (window.mermaid) return Promise.resolve()
    if (!loading) {
      loading = fetch(base + 'mermaid.min.js')
        .then(function (res) {
          if (!res.ok) throw new Error('HTTP ' + res.status)
          return res.text()
        })
        .then(function (code) {
          // 间接 eval：全局作用域执行；UMD 产物 this === globalThis，挂出 window.mermaid
          ;(0, eval)(code)
          if (!window.mermaid) throw new Error('mermaid 全局对象未生成')
        })
        .catch(function (err) {
          loading = null // 允许之后（如路由切换后）重试
          throw err
        })
    }
    return loading
  }

  function escapeHtml(text) {
    return text.replace(/[<>&]/g, function (c) {
      return { '<': '&lt;', '>': '&gt;', '&': '&amp;' }[c]
    })
  }

  // force = true 时对已渲染过的容器也重渲染（深浅色切换用）；
  // observer / 路由触发传 false：没有新的未转换代码块就直接跳过，
  // 避免渲染产物再触发 observer 形成死循环。
  window.__mmRender = function (isDark, force) {
    var blocks = document.querySelectorAll('div.language-mermaid')
    if (!blocks.length && !force) return Promise.resolve()

    return loadMermaid()
      .catch(function (err) {
        console.warn('[mermaid] 加载失败：', err)
      })
      .then(function () {
        if (!window.mermaid) return
        var mermaid = window.mermaid
        mermaid.initialize({ startOnLoad: false, theme: isDark ? 'dark' : 'default' })

        // 首次渲染：把 ```mermaid 代码块整个换成容器，源码存 data 属性（供主题切换时重渲染）。
        Array.prototype.forEach.call(blocks, function (block) {
          var box = document.createElement('div')
          box.className = 'mermaid-box'
          var codeEl = block.querySelector('code')
          box.dataset.mermaidSrc = (codeEl ? codeEl.textContent : '') || block.textContent || ''
          block.replaceWith(box)
        })

        // 容器统一渲染；已渲染且非强制的跳过。
        var boxes = document.querySelectorAll('.mermaid-box')
        return Promise.all(
          Array.prototype.map.call(boxes, function (box) {
            if (box.dataset.mermaidDone === '1' && !force) return Promise.resolve()
            var src = box.dataset.mermaidSrc || ''
            box.dataset.mermaidDone = '1'
            var id = 'mmd-' + Math.random().toString(36).slice(2, 10)
            return mermaid
              .render(id, src)
              .then(function (r) {
                box.innerHTML = r.svg
              })
              .catch(function (err) {
                console.warn('[mermaid] 渲染失败：', err)
                box.innerHTML =
                  '<pre class="mermaid-fallback">' + escapeHtml(src) + '</pre>'
              })
          })
        )
      })
  }
})()
