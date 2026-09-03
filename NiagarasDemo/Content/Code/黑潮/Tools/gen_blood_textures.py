# -*- coding: utf-8 -*-
"""
黑潮(The Black Audit)—— 血丝缠绕特效贴图生成器
参照图: 最前方人物身上的根须状自发光血丝(暗底 + 亮核 + 光晕 + 流动脉冲)

生成:
  T_BloodVeins_E.png      1024x1024  可无缝平铺血管网 (RGB=颜色, A=遮罩)
  T_BloodFlowNoise.png    512x512    可平铺 FBM 噪声 (R/G/B=三个尺度, UE 里关 sRGB)
  T_BloodTendril_2x2.png  2048x2048  2x2 血丝卷须变体 (SubUV 随机取样)
  T_BloodGlow_Sprite.png  512x512    柔光光斑 Sprite
  T_BloodRibbon.png       1024x256   Ribbon 拖尾条纹
  _preview.png            深色底预览拼图

用法: python gen_blood_textures.py
"""
import math
import os
import random

import numpy as np
from PIL import Image, ImageDraw, ImageFont

SEED = 20260903
HERE = os.path.dirname(os.path.abspath(__file__))
OUT_DIR = os.path.join(os.path.dirname(HERE), "Textures")

# ---------------- 颜色 ramp(参考图取色) ----------------
# 暗部 #2A0408 → 主色 #8B0F1A → 亮红 #FF3B2F → 亮核 #FFB3A0
RAMP_X = [0.00, 0.28, 0.55, 0.80, 1.00]
RAMP_C = [(42, 4, 8), (139, 15, 26), (198, 28, 34), (255, 59, 47), (255, 179, 160)]
PURPLE = (154, 75, 216)  # 端点冷紫 #9A4BD8


def ramp_rgb(e):
    """e: [0,1] 数组 → (h,w,3) uint8 颜色"""
    e = np.clip(e, 0.0, 1.0)
    ch = []
    for i in range(3):
        ys = [c[i] for c in RAMP_C]
        ch.append(np.interp(e, RAMP_X, ys))
    return np.stack(ch, axis=-1)


def smoothstep(t):
    t = np.clip(t, 0.0, 1.0)
    return t * t * (3.0 - 2.0 * t)


# ---------------- 无缝噪声 ----------------
def periodic_noise(h, w, cy, cx, rng):
    g = rng.random((cy, cx))
    ys = np.arange(h) * cy / h
    xs = np.arange(w) * cx / w
    yi = np.floor(ys).astype(int)
    xi = np.floor(xs).astype(int)
    fy = smoothstep(ys - yi)[:, None]
    fx = smoothstep(xs - xi)[None, :]
    y0 = np.mod(yi, cy)
    y1 = np.mod(yi + 1, cy)
    x0 = np.mod(xi, cx)
    x1 = np.mod(xi + 1, cx)
    v00 = g[y0][:, x0]
    v01 = g[y0][:, x1]
    v10 = g[y1][:, x0]
    v11 = g[y1][:, x1]
    return v00 * (1 - fy) * (1 - fx) + v01 * (1 - fy) * fx + v10 * fy * (1 - fx) + v11 * fy * fx


def fbm(h, w, cy, cx, octaves, rng):
    total = np.zeros((h, w))
    amp, norm = 1.0, 0.0
    for o in range(octaves):
        total += amp * periodic_noise(h, w, cy * (2 ** o), cx * (2 ** o), rng)
        norm += amp
        amp *= 0.5
    return total / norm


# ---------------- 线段光栅(锐核 + 光晕) ----------------
def draw_seg(core, halo, tipl, x0, y0, x1, y1, b, wc, wh, tw):
    """把一条线段画进 core/halo/tip 三个浮点层;越界部分用环绕副本,保证可平铺"""
    H, W = core.shape
    margin = wh * 3.0 + 2.0
    for ox in (-W, 0, W):
        for oy in (-H, 0, H):
            xa, ya, xb, yb = x0 + ox, y0 + oy, x1 + ox, y1 + oy
            minx = int(math.floor(min(xa, xb) - margin))
            maxx = int(math.ceil(max(xa, xb) + margin))
            if maxx < 0 or minx >= W:
                continue
            miny = int(math.floor(min(ya, yb) - margin))
            maxy = int(math.ceil(max(ya, yb) + margin))
            if maxy < 0 or miny >= H:
                continue
            minx, miny = max(minx, 0), max(miny, 0)
            maxx, maxy = min(maxx, W - 1), min(maxy, H - 1)
            gx = np.arange(minx, maxx + 1, dtype=np.float64)[None, :]
            gy = np.arange(miny, maxy + 1, dtype=np.float64)[:, None]
            dx, dy = xb - xa, yb - ya
            L2 = dx * dx + dy * dy
            if L2 < 1e-9:
                d2 = (gx - xa) ** 2 + (gy - ya) ** 2
            else:
                t = np.clip(((gx - xa) * dx + (gy - ya) * dy) / L2, 0.0, 1.0)
                d2 = (gx - (xa + t * dx)) ** 2 + (gy - (ya + t * dy)) ** 2
            reg = core[miny:maxy + 1, minx:maxx + 1]
            np.maximum(reg, b * np.exp(-d2 / (wc * wc)), out=reg)
            reg = halo[miny:maxy + 1, minx:maxx + 1]
            reg += (b * 0.55) * np.exp(-d2 / (wh * wh))
            if tw > 0.0:
                reg = tipl[miny:maxy + 1, minx:maxx + 1]
                np.maximum(reg, tw * np.exp(-d2 / (max(wc, 1.3) ** 2)), out=reg)


# ---------------- 根须生长 ----------------
def grow(core, halo, tipl, walkers, rng, budget, wrap, curl, step,
         wmin, pulse_freq, end_fade):
    """walkers: 初始行走者列表;返回 (节点列表)"""
    H, W = core.shape
    stack = [dict(w) for w in walkers]
    nodes = []
    n_seg = 0
    while stack and n_seg < budget:
        wk = stack.pop()
        x, y, ang = wk["x"], wk["y"], wk["ang"]
        w, b = wk["w"], wk["b"]
        life = wk["life"]
        dist0 = wk.get("dist", 0.0)
        phase = wk["phase"]
        sf = wk["sf"]
        for i in range(life):
            u = i / max(life - 1, 1)
            # 游走:正弦摆动 + 随机抖动
            ang += math.sin((dist0 + i * step) * sf + phase) * curl
            ang += rng.uniform(-0.09, 0.09)
            nx = x + math.cos(ang) * step
            ny = y + math.sin(ang) * step
            # 沿线的脉冲亮度(流动感)
            pulse = 0.55 + 0.45 * math.sin((dist0 + i * step) * pulse_freq + phase * 1.7)
            taper = 1.0 - 0.35 * u
            fade = 1.0
            if end_fade:
                fade = smoothstep(u / 0.06) * (1.0 - smoothstep((u - 0.72) / 0.28))
            bright = b * pulse * taper * fade
            wc = max(0.7, w * 0.40)
            wh = max(2.5, w * 1.8)
            tw = 0.0
            if wk["depth"] >= 3:
                tw = 0.45
            if u > 0.65:
                tw = max(tw, smoothstep((u - 0.65) / 0.35))
            draw_seg(core, halo, tipl, x, y, nx, ny, bright, wc, wh, tw * fade)
            n_seg += 1
            # 分叉
            rem = life - i
            if (rng.random() < wk["pb"] and wk["depth"] < 6
                    and n_seg < budget and rem > 8 and w * 0.62 > wmin):
                nodes.append((x, y, 0.30 + 0.08 * (6 - wk["depth"]), 1.8))
                for s in (1, -1):
                    stack.append(dict(
                        x=x, y=y, ang=ang + s * rng.uniform(0.55, 1.1),
                        w=w * 0.62, b=b * 0.88, life=int(rem * 0.5),
                        pb=wk["pb"] * 0.85, depth=wk["depth"] + 1,
                        phase=rng.uniform(0, math.tau), sf=sf * 1.35,
                        dist=dist0 + i * step))
            if w < wmin:
                break
            x, y = nx, ny
            if wrap:
                x %= W
                y %= H
        # 末端发光节点
        nodes.append((x, y, 0.55 * b, 1.6))
    return nodes


# ---------------- 1) 主血管网(可平铺) ----------------
def gen_veins(size=1024):
    rng = np.random.default_rng(SEED)
    core = np.zeros((size, size))
    halo = np.zeros((size, size))
    tipl = np.zeros((size, size))

    walkers = []
    n_root = 9
    for i in range(n_root):
        walkers.append(dict(
            x=(i + 0.5) * size / n_root + rng.uniform(-22, 22),
            y=size + rng.uniform(-30, 30),
            ang=-math.pi / 2 + rng.uniform(-0.5, 0.5),
            w=5.4 + rng.uniform(-0.9, 0.9), b=1.0,
            life=170, pb=0.030, depth=0,
            phase=rng.uniform(0, math.tau),
            sf=rng.uniform(0.010, 0.022)))
    # 背景休眠细网(低亮度)
    for _ in range(14):
        walkers.append(dict(
            x=rng.uniform(0, size), y=rng.uniform(0, size),
            ang=rng.uniform(-math.pi, math.pi),
            w=1.3, b=0.16, life=90, pb=0.020, depth=3,
            phase=rng.uniform(0, math.tau),
            sf=rng.uniform(0.012, 0.030)))

    nodes = grow(core, halo, tipl, walkers, rng,
                 budget=8000, wrap=True, curl=0.16, step=7.0,
                 wmin=0.55, pulse_freq=0.030, end_fade=False)
    # grow 里 draw_seg 已画,节点直接补进 core
    for (x, y, amp, sig) in nodes:
        draw_seg(core, halo, tipl, x, y, x, y, amp, sig, sig, 0.0)

    e = np.clip(core * 0.85, 0.0, 1.0)
    h = np.clip(halo * 0.50, 0.0, 1.0)
    t = np.clip(tipl, 0.0, 1.0)
    rgb = ramp_rgb(e).astype(np.float64)
    # 光晕补一点环境红
    rgb += h[..., None] * np.array([120, 14, 20]) * 0.55
    # 端点偏紫
    rgb = rgb * (1 - t[..., None] * 0.38) + np.array(PURPLE) * (t[..., None] * 0.38)
    alpha = np.clip(core * 0.85 + halo * 0.30, 0.0, 1.0)
    out = np.dstack([np.clip(rgb, 0, 255), alpha * 255.0]).astype(np.uint8)
    return Image.fromarray(out, "RGBA")


# ---------------- 2) 卷须 Sprite(2x2 变体) ----------------
def gen_tendril_cell(cell=1024, seed_off=0, curl=0.30, life=200, w0=6.0):
    rng = np.random.default_rng(SEED + 1000 + seed_off)
    core = np.zeros((cell, cell))
    halo = np.zeros((cell, cell))
    tipl = np.zeros((cell, cell))
    walkers = [dict(
        x=cell * 0.5 + rng.uniform(-60, 60), y=cell * 0.97,
        ang=-math.pi / 2 + rng.uniform(-0.3, 0.3),
        w=w0, b=1.0, life=life, pb=0.022, depth=0,
        phase=rng.uniform(0, math.tau), sf=rng.uniform(0.006, 0.014))]
    nodes = grow(core, halo, tipl, walkers, rng,
                 budget=2600, wrap=False, curl=curl, step=6.0,
                 wmin=0.55, pulse_freq=0.040, end_fade=True)
    for (x, y, amp, sig) in nodes:
        if amp > 0.2:
            draw_seg(core, halo, tipl, x, y, x, y, amp, sig, sig, 0.0)
    e = np.clip(core * 0.9, 0.0, 1.0)
    t = np.clip(tipl * 1.2, 0.0, 1.0)
    rgb = ramp_rgb(e).astype(np.float64)
    rgb = rgb * (1 - t[..., None] * 0.45) + np.array(PURPLE) * (t[..., None] * 0.45)
    alpha = np.clip(core * 0.95 + halo * 0.22, 0.0, 1.0)
    return np.dstack([np.clip(rgb, 0, 255), alpha * 255.0]).astype(np.uint8)


def gen_tendrils(cell=1024):
    variants = [
        dict(seed_off=0, curl=0.30, life=210, w0=6.0),
        dict(seed_off=1, curl=0.46, life=170, w0=5.2),
        dict(seed_off=2, curl=0.20, life=240, w0=6.5),
        dict(seed_off=3, curl=0.38, life=190, w0=5.0),
    ]
    grid = np.zeros((cell * 2, cell * 2, 4), dtype=np.uint8)
    for i, v in enumerate(variants):
        c = gen_tendril_cell(cell, **v)
        y0 = (i // 2) * cell
        x0 = (i % 2) * cell
        grid[y0:y0 + cell, x0:x0 + cell] = c
    return Image.fromarray(grid, "RGBA")


# ---------------- 3) 柔光光斑 ----------------
def gen_glow(size=512):
    rng = np.random.default_rng(SEED + 2000)
    yy, xx = np.mgrid[0:size, 0:size].astype(np.float64)
    c = size / 2.0
    r = np.sqrt((xx - c) ** 2 + (yy - c) ** 2) / c
    n = fbm(size, size, 6, 6, 3, rng)
    g = (1.00 * np.exp(-(r / 0.11) ** 2)
         + 0.50 * np.exp(-(r / 0.30) ** 2)
         + 0.18 * np.exp(-(r / 0.62) ** 2))
    e = np.clip(g * (0.90 + 0.30 * (n - 0.5)), 0.0, 1.0)
    rgb = ramp_rgb(e)
    alpha = np.clip(g * 1.05, 0.0, 1.0)
    return Image.fromarray(np.dstack([rgb, alpha * 255.0]).astype(np.uint8), "RGBA")


# ---------------- 4) Ribbon 拖尾 ----------------
def gen_ribbon(w=1024, h=256):
    rng = np.random.default_rng(SEED + 3000)
    vv = np.arange(h) / (h - 1) - 0.5
    pcore = np.exp(-(vv / 0.13) ** 2)
    phalo = 0.42 * np.exp(-(vv / 0.38) ** 2)
    profile = pcore + phalo
    n = fbm(1, w, 1, 22, 4, rng)[0]
    bright = 0.62 + 0.55 * (n - 0.5)
    uu = np.arange(w) / (w - 1)
    taper = smoothstep(uu / 0.05) * (1.0 - smoothstep((uu - 0.95) / 0.05))
    e = np.clip(profile[:, None] * bright[None, :] * taper[None, :], 0.0, 1.0)
    rgb = ramp_rgb(e)
    alpha = np.clip(e * 1.05, 0.0, 1.0)
    return Image.fromarray(np.dstack([rgb, alpha * 255.0]).astype(np.uint8), "RGBA")


# ---------------- 5) 流动噪声 ----------------
def gen_flow_noise(size=512):
    rng = np.random.default_rng(SEED + 4000)
    chs = [fbm(size, size, 24, 24, 3, rng),
           fbm(size, size, 12, 12, 3, rng),
           fbm(size, size, 6, 6, 3, rng)]
    arr = np.stack(chs, axis=-1)
    return Image.fromarray((arr * 255).astype(np.uint8), "RGB")


# ---------------- 预览 ----------------
def _label(draw, xy, text):
    try:
        font = ImageFont.load_default(16)
    except TypeError:
        font = ImageFont.load_default()
    draw.text(xy, text, fill=(160, 160, 170), font=font)


def make_preview(textures):
    bg = np.array([10, 14, 20], dtype=np.float64)
    items = [
        ("T_BloodVeins_E", textures["veins"].resize((512, 512), Image.LANCZOS)),
        ("T_BloodFlowNoise (sRGB off)", textures["noise"].resize((512, 512), Image.LANCZOS)),
        ("T_BloodTendril_2x2 (cell 0)",
         textures["tendril"].crop((0, 0, 1024, 1024)).resize((512, 512), Image.LANCZOS)),
        ("T_BloodGlow_Sprite", textures["glow"].resize((256, 256), Image.LANCZOS)),
        ("T_BloodRibbon", textures["ribbon"].resize((512, 128), Image.LANCZOS)),
    ]
    W = 20 + sum(im.width for _, im in items) + 20 * (len(items) - 1)
    H = 640
    sheet = np.tile(bg, (H, W, 1))
    x = 20
    for _, im in items:
        a = np.asarray(im.convert("RGBA"), dtype=np.float64)
        h, w = a.shape[:2]
        y = 40
        if h < 512:
            y = 40 + (512 - h) // 2
        region = sheet[y:y + h, x:x + w]
        sheet[y:y + h, x:x + w] = np.clip(
            region + a[..., :3] * (a[..., 3:4] / 255.0), 0, 255)
        x += w + 20
    img = Image.fromarray(sheet.astype(np.uint8))
    d = ImageDraw.Draw(img)
    x = 20
    for name, im in items:
        _label(d, (x, 600 if im.height >= 512 else 590 + 0), name)
        x += im.width + 20
    return img


def main():
    os.makedirs(OUT_DIR, exist_ok=True)
    textures = {}
    textures["veins"] = gen_veins(1024)
    textures["noise"] = gen_flow_noise(512)
    textures["tendril"] = gen_tendrils(1024)
    textures["glow"] = gen_glow(512)
    textures["ribbon"] = gen_ribbon(1024, 256)

    names = {
        "veins": "T_BloodVeins_E.png",
        "noise": "T_BloodFlowNoise.png",
        "tendril": "T_BloodTendril_2x2.png",
        "glow": "T_BloodGlow_Sprite.png",
        "ribbon": "T_BloodRibbon.png",
    }
    for k, im in textures.items():
        p = os.path.join(OUT_DIR, names[k])
        im.save(p)
        print("saved:", p)

    prev = make_preview(textures)
    pp = os.path.join(HERE, "_preview.png")
    prev.save(pp)
    print("preview:", pp)


if __name__ == "__main__":
    main()
