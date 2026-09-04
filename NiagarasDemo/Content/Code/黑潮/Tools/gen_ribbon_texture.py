# -*- coding: utf-8 -*-
"""
黑潮(The Black Tide)—— 单根丝带贴图生成器
参照 T_BloodVeins_E.png 的「暗底 + 亮核 + 光晕」发光风格, 生成一根丝带:
  - 只有一根: Catmull-Rom 脊线, 底部入场 → 上扬 S 弯 → 一个整圆环(旋转回环) → 右下甩尾 → 上挑收尖
  - 弯曲拧转: 宽度沿弧长周期收放(模拟丝带拧转), 收窄处能量集中更亮; 两端收成发丝尖
输出: Textures/T_Ribbon_E.png  1024x1024 RGBA (RGB=颜色, A=发光遮罩)
用法: python gen_ribbon_texture.py
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

# 丝带脊线控制点(1024 空间): 入场→S弯→圆环(旋转)→甩尾→上挑尖
CP = [
    (95, 905), (150, 730), (262, 555), (392, 470),
    (447, 464), (609, 419), (629, 324), (564, 249), (462, 244), (397, 317),
    (403, 419), (489, 477),
    (625, 515), (752, 596), (860, 588), (918, 472), (915, 330),
]
N = 1500  # 弧长均匀采样数


def smoothstep(t):
    t = np.clip(t, 0.0, 1.0)
    return t * t * (3.0 - 2.0 * t)


def catmull_rom(pts, per=56):
    p = [pts[0]] + pts + [pts[-1]]
    out = []
    for i in range(len(p) - 3):
        p0, p1, p2, p3 = (np.asarray(q, dtype=float) for q in (p[i], p[i + 1], p[i + 2], p[i + 3]))
        for j in range(per):
            t = j / per
            t2, t3 = t * t, t * t * t
            out.append(0.5 * ((2 * p1) + (-p0 + p2) * t
                              + (2 * p0 - 5 * p1 + 4 * p2 - p3) * t2
                              + (-p0 + 3 * p1 - 3 * p2 + p3) * t3))
    out.append(np.asarray(pts[-1], dtype=float))
    return np.array(out)


def arc_uniform(pts, n):
    """按弧长均匀重采样"""
    seg = np.linalg.norm(np.diff(pts, axis=0), axis=1)
    s = np.concatenate([[0.0], np.cumsum(seg)])
    s /= s[-1]
    tgt = np.linspace(0.0, 1.0, n)
    return np.stack([np.interp(tgt, s, pts[:, 0]), np.interp(tgt, s, pts[:, 1])], axis=1)


# ---------------- 脊线 → 每点的半宽 / 亮度 ----------------
spine = arc_uniform(catmull_rom(CP), N)
u = np.linspace(0.0, 1.0, N)

twist = np.abs(np.sin(np.pi * 4.0 * u + 0.6))          # 拧转: 1=正对宽面, 0=侧对收窄
taper = smoothstep(np.clip(u / 0.10, 0, 1)) * smoothstep(np.clip((1 - u) / 0.16, 0, 1))
halfw = 38.0 * taper * (0.18 + 0.82 * twist)
halfw += 1.3 * smoothstep(np.clip((u + 0.02) / 0.05, 0, 1)) * smoothstep(np.clip(((1 - u) + 0.02) / 0.05, 0, 1))
pinch = (1.0 - twist) ** 1.5                            # 收窄处更热
bright = (1.0 + 0.55 * pinch
          + 0.16 * np.sin(2 * np.pi * 2.3 * u + 1.3)
          + 0.09 * np.sin(2 * np.pi * 4.9 * u + 4.1)
          + 1.5 * np.exp(-(((1 - u) / 0.035) ** 2))     # 尾端亮头
          + 0.5 * np.exp(-((u / 0.05) ** 2)))           # 起端微亮

# ---------------- 最近距离场: 每像素到脊线的距离 + 弧长参数 ----------------
margin = 90.0
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
streak = 0.86 + 0.27 * np.sin(2 * np.pi * 15.0 * umap + 3.1 * np.sin(2 * np.pi * 2.2 * umap + 1.0))
E = body * (0.58 + 0.42 * core) * b_map * streak
E += 0.30 * np.exp(-((np.maximum(dist - w_map * 0.25, 0.0) / (w_map * 1.15 + 20.0)) ** 2)) \
    * np.clip(b_map, 0.0, 1.5)
E[~np.isfinite(E)] = 0.0

# ---------------- 亮点节点(尾端亮头 + 沿途火花) ----------------
rng = np.random.default_rng(SEED)
gx, gy = np.meshgrid(np.arange(SIZE, dtype=np.float32), np.arange(SIZE, dtype=np.float32))
nodes = [(0.99, 1.9, 15.0), (0.015, 0.8, 9.0), (0.36, 1.1, 11.0), (0.63, 1.0, 10.0)]
for un, amp, rad in nodes:
    cx, cy = spine[int(un * (N - 1))]
    E += amp * np.exp(-(((gx - cx) ** 2 + (gy - cy) ** 2) / (rad * rad)))

# ---------------- 泛光 bloom(3 次盒式滤波 ≈ 高斯) ----------------
def _box_axis(a, r, axis):
    pad = [(0, 0)] * a.ndim
    pad[axis] = (r, r)
    p = np.pad(a, pad, mode="edge")
    c = np.cumsum(p, axis=axis)
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


Etot = E + 0.55 * gauss_blur(E, 9) + 0.25 * gauss_blur(E, 30)


def ramp_rgb(e):
    e = np.clip(e, 0.0, 1.0)
    ch = [np.interp(e, RAMP_X, [c[i] for c in RAMP_C]).astype(np.float32) for i in range(3)]
    return np.stack(ch, axis=-1) / 255.0


emissive = ramp_rgb(1.0 - np.exp(-Etot * 0.85))
rgb = np.clip(BG + emissive + rng.normal(0.0, 0.004, (SIZE, SIZE, 1)).astype(np.float32), 0.0, 1.0)
alpha = np.clip(E * 0.8, 0.0, 1.0)

out = np.dstack([(rgb * 255 + 0.5).astype(np.uint8), (alpha * 255 + 0.5).astype(np.uint8)])
os.makedirs(OUT_DIR, exist_ok=True)
path = os.path.join(OUT_DIR, "T_Ribbon_E.png")
Image.fromarray(out, "RGBA").save(path)
print("saved:", path)
