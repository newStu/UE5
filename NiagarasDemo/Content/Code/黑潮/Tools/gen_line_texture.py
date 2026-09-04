# -*- coding: utf-8 -*-
"""
黑潮(The Black Tide)—— 直线贴图生成器
生成一张透明背景 PNG, 内容只有一条水平直线:
  - 横向贯穿整图(左右不衰减, 可沿 U 方向平铺), 用于丝带/光束材质
  - 垂直方向: 锐边直线 + 亮核, 配合同款红色发光 Ramp
输出: Textures/T_Line_E.png  1024x1024 RGBA (RGB=颜色, A=直线遮罩, 背景全透明)
用法: python gen_line_texture.py
"""
import os

import numpy as np
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
OUT_DIR = os.path.join(os.path.dirname(HERE), "Textures")
SIZE = 1024

HALF_W = 7.0  # 直线半厚(像素), 总厚约 14px

# 与 gen_ribbon_texture.py 同一色系: 暗部 → 主红 → 亮红 → 亮核
RAMP_X = [0.00, 0.30, 0.58, 0.82, 1.00]
RAMP_C = [(42, 4, 8), (139, 15, 26), (198, 28, 34), (255, 59, 47), (255, 190, 165)]


def smoothstep(t):
    t = np.clip(t, 0.0, 1.0)
    return t * t * (3.0 - 2.0 * t)


# ---------------- 垂直截面: 直线本体 + 亮核 ----------------
gy = np.arange(SIZE, dtype=np.float32)[:, None]
dist = np.abs(gy - (SIZE - 1) / 2.0)          # 每行到水平中线的距离
t = dist / HALF_W
body = smoothstep(np.clip((1.0 - t) / 0.35, 0.0, 1.0))   # 主体(抗锯齿锐边)
core = np.exp(-((t / 0.45) ** 2))                         # 中央亮核
E = body * (0.62 + 0.38 * core)
E = np.repeat(E, SIZE, axis=1)                            # 横向完全均匀


def ramp_rgb(e):
    e = np.clip(e, 0.0, 1.0)
    ch = [np.interp(e, RAMP_X, [c[i] for c in RAMP_C]).astype(np.float32) for i in range(3)]
    return np.stack(ch, axis=-1) / 255.0


rgb = ramp_rgb(1.0 - np.exp(-E * 1.1))
alpha = np.clip(E, 0.0, 1.0)                              # 背景全透明

out = np.dstack([(rgb * 255 + 0.5).astype(np.uint8), (alpha * 255 + 0.5).astype(np.uint8)])
os.makedirs(OUT_DIR, exist_ok=True)
path = os.path.join(OUT_DIR, "T_Line_E.png")
Image.fromarray(out, "RGBA").save(path)
print("saved:", path)
