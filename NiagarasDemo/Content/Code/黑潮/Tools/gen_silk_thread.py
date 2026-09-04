# -*- coding: utf-8 -*-
"""
黑潮(The Black Tide)—— S 型丝线贴图生成器
参照 T_BloodVeins_E.png 的「暗底 + 亮核 + 光晕」发光风格, 生成一根丝线:
  - 只有一根丝线, 细线宽(非宽带), 沿对角线走一整条正弦 S 弯
  - 亮度沿线流动脉冲, 两端收成发丝尖, 尾端带亮头
输出: Textures/T_SilkThread_E.png  1024x1024 RGBA (RGB=颜色, A=发光遮罩)
用法: python gen_silk_thread.py
"""
import os

import numpy as np
from PIL import Image

SEED = 20260904
HERE = os.path.dirname(os.path.abspath(__file__))
OUT_DIR = os.path.join(os.path.dirname(HERE), "Textures")
SIZE = 1024

# 与 gen_blood_textures.py 同一色系: 暗部 → 主红 → 亮红 → 亮核
RAMP_X = [0.00, 0.30, 0.58, 0.82, 1.00]
RAMP_C = [(42, 4, 8), (139, 15, 26), (198, 28, 34), (255, 59, 47), (255, 190, 165)]
BG = np.array([30, 4, 9], dtype=np.float32) / 255.0

N = 1500  # 弧长均匀采样数


def smoothstep(t):
    t = np.clip(t, 0.0, 1.0)
    return t * t * (3.0 - 2.0 * t)


def arc_uniform(pts, n):
    """按弧长均匀重采样"""
    seg = np.linalg.norm(np.diff(pts, axis=0), axis=1)
    s = np.concatenate([[0.0], np.cumsum(seg)])
    s /= s[-1]
    tgt = np.linspace(0.0, 1.0, n)
    return np.stack([np.interp(tgt, s, pts[:, 0]), np.interp(tgt, s, pts[:, 1])], axis=1)


# ---------------- S 型脊线: 对角主轴 + 一个整周期正弦 + 次级摆动 ----------------
C = np.array([512.0, 512.0])
d = np.array([0.7071, -0.7071])   # 主轴: 左下 → 右上
p = np.array([0.7071, 0.7071])    # 垂直于主轴
L, A, A2 = 1150.0, 140.0, 42.0    # 轴长 / 主摆幅 / 次级摆幅
tp = np.linspace(0.0, 1.0, 4000)
perp = A * np.sin(2 * np.pi * tp) + A2 * np.sin(6 * np.pi * tp + 1.0) * np.sin(np.pi * tp)
spine_raw = C[None, :] + d[None, :] * ((tp - 0.5) * L)[:, None] + p[None, :] * perp[:, None]
spine = arc_uniform(spine_raw, N)
u = np.linspace(0.0, 1.0, N)

# ---------------- 线宽 / 亮度 ----------------
taper = smoothstep(np.clip(u / 0.12, 0, 1)) * smoothstep(np.clip((1 - u) / 0.12, 0, 1))
halfw = taper * (4.0 + 1.0 * np.sin(2 * np.pi * 5.0 * u + 0.8))          # 丝线: 细且微呼吸
bright = (0.95
          + 0.30 * np.sin(2 * np.pi * 3.0 * u - 1.2)                     # 流动脉冲
          + 0.15 * np.sin(2 * np.pi * 7.0 * u + 2.6)
          + 1.6 * np.exp(-(((1 - u) / 0.04) ** 2))                       # 尾端亮头
          + 0.6 * np.exp(-((u / 0.05) ** 2)))                            # 起端微亮

# ---------------- 最近距离场: 每像素到脊线的距离 + 弧长参数 ----------------
margin = 46.0
dist = np.full((SIZE, SIZE), np.inf, dtype=np.float32)
umap = np.zeros((SIZE, SIZE), dtype=np.float32)
for i in range(N - 1):
    x0, y0 = spine[i]
    x1, y1 = spine[i + 1]
    xa = int(max(min(x0, x1) - margin, 0)); xb = int(min(max(x0, x1) + margin, SIZE - 1))
    ya = int(max(min(y0, y1) - margin, 0)); yb = int(min(max(y0, y1) + margin, SIZE - 1))
    gx = np.arange(xa, xb + 1, dtype=np.float32)[None, :]
    gy = np.arange(ya, yb + 1, dtype=np.float32)[:, None]
    dx, dy = x1 - x0, y1 - y0
    L2 = max(dx * dx + dy * dy, 1e-9)
    s = np.clip(((gx - x0) * dx + (gy - y0) * dy) / L2, 0.0, 1.0)
    d2 = (gx - (x0 + s * dx)) ** 2 + (gy - (y0 + s * dy)) ** 2
    reg = dist[ya:yb + 1, xa:xb + 1]
    m = d2 < reg
    reg[m] = np.sqrt(d2[m])
    umap[ya:yb + 1, xa:xb + 1][m] = 0.5 * (u[i] + u[i + 1])

w_map = np.interp(umap.ravel(), u, halfw).reshape(SIZE, SIZE).astype(np.float32)
b_map = np.interp(umap.ravel(), u, bright).reshape(SIZE, SIZE).astype(np.float32)

# ---------------- 横截面: 锐核 + 羽化边 + 光晕 ----------------
t = dist / np.maximum(w_map, 1e-3)
body = smoothstep(np.clip((1.0 - t) / 0.42, 0.0, 1.0))
core = np.exp(-((t / 0.36) ** 2))
streak = 0.86 + 0.27 * np.sin(2 * np.pi * 22.0 * umap + 3.1 * np.sin(2 * np.pi * 2.2 * umap + 1.0))
E = body * (0.58 + 0.42 * core) * b_map * streak
E += 0.35 * np.exp(-((np.maximum(dist - w_map * 0.25, 0.0) / (w_map * 1.15 + 18.0)) ** 2)) \
    * np.clip(b_map, 0.0, 1.5)
E[~np.isfinite(E)] = 0.0

# ---------------- 亮点节点(尾端亮头 + 沿途火花) ----------------
rng = np.random.default_rng(SEED)
gx, gy = np.meshgrid(np.arange(SIZE, dtype=np.float32), np.arange(SIZE, dtype=np.float32))
nodes = [(0.99, 1.9, 13.0), (0.01, 0.8, 9.0), (0.30, 1.0, 10.0), (0.58, 1.1, 10.0), (0.82, 0.9, 9.0)]
for un, amp, rad in nodes:
    cx, cy = spine[int(un * (N - 1))]
    E += amp * np.exp(-(((gx - cx) ** 2 + (gy - cy) ** 2) / (rad * rad)))

# ---------------- 泛光 bloom(3 次盒式滤波 ≈ 高斯) ----------------
def _box_axis(a, r, axis):
    pad = [(0, 0)] * a.ndim
    pad[axis] = (r, r)
    pn = np.pad(a, pad, mode="edge")
    c = np.cumsum(pn, axis=axis)
    zshape = list(a.shape)
    zshape[axis] = 1
    c = np.concatenate([np.zeros(zshape, dtype=c.dtype), c], axis=axis)
    n = a.shape[axis]
    hi = np.take(c, np.arange(2 * r + 1, n + 2 * r + 1), axis=axis)
    lo = np.take(c, np.arange(0, n), axis=axis)
    return (hi - lo) / (2 * r + 1)


def gauss_blur(a, sigma):
    r = max(1, int(round(sigma)))
    out = a.astype(np.float32)
    for _ in range(3):
        out = _box_axis(out, r, 1)
        out = _box_axis(out, r, 0)
    return out


Etot = E + 0.55 * gauss_blur(E, 7) + 0.25 * gauss_blur(E, 24)


def ramp_rgb(e):
    e = np.clip(e, 0.0, 1.0)
    ch = [np.interp(e, RAMP_X, [c[i] for c in RAMP_C]).astype(np.float32) for i in range(3)]
    return np.stack(ch, axis=-1) / 255.0


emissive = ramp_rgb(1.0 - np.exp(-Etot * 0.85))
rgb = np.clip(BG + emissive + rng.normal(0.0, 0.004, (SIZE, SIZE, 1)).astype(np.float32), 0.0, 1.0)
alpha = np.clip(E * 0.8, 0.0, 1.0)

out = np.dstack([(rgb * 255 + 0.5).astype(np.uint8), (alpha * 255 + 0.5).astype(np.uint8)])
os.makedirs(OUT_DIR, exist_ok=True)
path = os.path.join(OUT_DIR, "T_SilkThread_E.png")
Image.fromarray(out, "RGBA").save(path)
print("saved:", path)
