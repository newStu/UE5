# UE5 学习/演示工程仓库

本仓库包含两个 **Unreal Engine 5.8** 工程（纯蓝图，无 C++ Source）：

| 工程 | 说明 |
|---|---|
| `ChallengeGame/` | 挑战游戏工程（含自定义关卡 `Level_Scene_01`、角色/输入/机关蓝图） |
| `Demo 5.8/` | 演示工程（第三人称、输入、`Variant_Combat` 战斗 AI/连招蓝图） |

共用插件：`ModelingToolsEditorMode`；`Demo 5.8` 另启用 `GameplayStateTree`。

---

## ⚠️ 这个仓库不包含大资产（重要）

为避免把仓库撑爆（大资产曾被直接提交导致 `.git` 膨胀到 1.6GB），本仓库**只版本化代码/蓝图/配置/文档**，所有大资产**不在 git 中**，需要在新机器上手动恢复。

### ✅ 跟踪的内容
- `*.uproject`、`Config/`（引擎配置）
- `Content/Code/`（你的蓝图）
- `Content/Input/`（输入配置）
- `Content/ThirdPerson/`（第三人称设置）
- `Content/Variant_Combat/`（你写的战斗蓝图：AI、连招、EQS、StateTree）
- `Docs/`（操作文档截图，仓库根目录）

### ❌ 未跟踪、需手动恢复的内容
| 路径 | 大小 | 来源 / 恢复方式 |
|---|---|---|
| `ChallengeGame/Content/ChallengeGame/` | **361M** | **你自己的关卡 + 导入的大贴图。请从你自己的独立备份（zip/网盘）解压回原路径。** |
| `ChallengeGame/Content/RPG_Character/` | 1.3G | 第三方 RPG 角色包，从原下载来源重新导入。 |
| `Demo 5.8/Content/Characters/` (Mannequins) | 126M | Epic 官方骨架，随引擎 / 从 Fab 重新添加。 |
| `Demo 5.8/Content/Fab/` | 63M | Fab 商店资产，重新下载导入。 |
| `Binaries/ Intermediate/ Saved/ DerivedDataCache/` | — | 引擎自动生成，**不要恢复**，打开工程会重建。 |

---

## 在新机器上运行

1. **安装 Unreal Engine 5.8**（通过 Epic Games Launcher → Unreal Engine 库）。
2. **克隆本仓库**：
   ```bash
   git clone <仓库地址>
   ```
3. **恢复大资产**（见上表）：
   - 把你的 `ChallengeGame/Content/ChallengeGame/` 场景备份 zip 解压回原路径；
   - 按 `+.uproject` 里启用的插件，重新导入 RPG 角色包、Mannequins、Fab 资产到对应 `Content/` 子目录。
4. **生成工程文件**：右键 `*.uproject` → **Generate Visual Studio project files**。
5. **打开工程**：双击 `*.uproject` 启动编辑器；首次打开会编译蓝图并重建 `DerivedDataCache`，耐心等待。

---

## 给开发者：后续提交注意事项

- **只提交**蓝图、配置、代码、文档等小文件。
- **绝不提交** `Binaries/`、`Intermediate/`、`Saved/`、`DerivedDataCache/`（已被 `.gitignore` 挡住）。
- 新建的大资产文件夹默认会被 `.gitignore` 忽略（`**/Content/*` 规则）。如果新增了**自己的小蓝图目录**需要跟踪，在 `.gitignore` 的白名单段加一行 `!**/Content/<你的目录>/`。
- 提交前可用 `git status` 确认没有大文件混入：
  ```bash
  git status --short | grep -E "\.(uasset|umap)$"   # 只应出现白名单目录内的蓝图
  ```

---

## 📚 在线文档（VitePress）

本仓库的中文操作文档已用 [VitePress](https://vitepress.dev) 打包成可导航、可搜索的文档站，并通过 GitHub Actions 自动构建发布到 GitHub Pages。

> **部署地址**：`https://<你的 GitHub 用户名>.github.io/ue5/`
> （在 GitHub 建好仓库、push 上去，并在 **Settings → Pages → Source** 选 **"GitHub Actions"** 后即生效）

### 本地预览

```bash
npm install
npm run docs:dev      # 启动本地开发服务器（热更新）
npm run docs:build    # 构建静态站点到 Docs/.vitepress/dist
```

### 文档结构

- 文档源在 [`Docs/`](Docs/) 目录 —— VitePress 工程根就在此，**新增文档直接在此加 markdown 即可**，图片和 md 放同一目录用相对路径引用。
- 顶部导航 / 侧边栏 / 搜索配置在 [`Docs/.vitepress/config.mts`](Docs/.vitepress/config.mts)。
- 自动构建流水线在 [`.github/workflows/deploy.yml`](.github/workflows/deploy.yml)（推送到 `master` 自动触发，无需手动操作）。

### 构建状态

![Deploy VitePress](https://github.com/<你的GitHub用户名>/ue5/actions/workflows/deploy.yml/badge.svg)
<!-- 把上面 URL 里的 <你的GitHub用户名> 换成你的实际 GitHub 用户名 -->
