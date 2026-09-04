# -*- coding: utf-8 -*-
"""
黑潮 —— 丝带 UV 流动材质 (M_流光) 生成脚本
在 UE 编辑器 Python 中运行, 自动创建(已存在则覆盖重建)材质:
  /Game/Code/MAT/黑潮流光/M_流光

材质内容 (Unlit + Additive + 双面):
  UV0 + Time*FlowSpeed → 贴图沿 U 方向持续流动 (默认 T_Line_E 直线贴图, 可换 T_Ribbon_E)
  贴图RGB × Tint × 顶点色 → 自发光 (Emissive)
  贴图A  × 顶点色A      → 不透明度 (Opacity), 配合 Niagara 的缩放颜色/Alpha 做淡入淡出

用法: UE 编辑器 → 菜单 工具(Tools) → 执行 Python 脚本(Execute Python Script) 选本文件
      或 输出日志(Output Log) 切到 Python, 输入: py "<本文件路径>"
"""
import unreal

MAT_PATH = "/Game/Code/MAT/黑潮流光"
MAT_NAME = "M_流光"

# 贴图参数默认值: 优先直线贴图, 不在则退回丝带贴图
TEX_CANDIDATES = [
    "/Game/Code/黑潮/Textures/T_Line_E",
    "/Game/Code/黑潮/Textures/T_Ribbon_E",
]


def connect(mel, a, a_outs, b, b_ins):
    """按候选引脚名依次尝试连接(不同版本节点引脚名略有差异), 成功返回 True"""
    for ao in a_outs:
        for bi in b_ins:
            if mel.connect_material_expressions(a, ao, b, bi):
                return True
    return False


def main():
    mel = unreal.MaterialEditingLibrary
    eal = unreal.EditorAssetLibrary
    at = unreal.AssetToolsHelpers.get_asset_tools()

    pkg = f"{MAT_PATH}/{MAT_NAME}"
    if eal.does_asset_exist(pkg):
        eal.delete_asset(pkg)  # 覆盖式重建, 方便反复调整脚本重跑

    mat = at.create_asset(MAT_NAME, MAT_PATH, unreal.Material, unreal.MaterialFactoryNew())
    mat.set_editor_property("blend_mode", unreal.BlendMode.ADDITIVE)
    mat.set_editor_property("shading_model", unreal.MaterialShadingModel.UNLIT)
    mat.set_editor_property("two_sided", True)
    mat.set_editor_property("used_with_niagara_ribbons", True)

    def ex(cls, x, y):
        return mel.create_material_expression(mat, cls, x, y)

    # --- UV 流动: UV0 + Time × FlowSpeed ---
    uv = ex(unreal.MaterialExpressionTextureCoordinate, -1100, 60)
    time = ex(unreal.MaterialExpressionTime, -1100, 380)
    speed = ex(unreal.MaterialExpressionScalarParameter, -1420, 470)
    speed.set_editor_property("parameter_name", "FlowSpeed")
    speed.set_editor_property("default_value", 0.4)
    spd_mul = ex(unreal.MaterialExpressionMultiply, -760, 400)
    spd_add = ex(unreal.MaterialExpressionAdd, -480, 140)
    connect(mel, time, ["Time", ""], spd_mul, ["A"])
    connect(mel, speed, ["", "Output"], spd_mul, ["B"])
    connect(mel, uv, ["Coordinates", "UV0", ""], spd_add, ["A"])
    connect(mel, spd_mul, ["", "Output"], spd_add, ["B"])

    # --- 丝带贴图 (参数化) ---
    tex = next((t for t in (unreal.load_asset(p) for p in TEX_CANDIDATES) if t), None)
    samp = ex(unreal.MaterialExpressionTextureSampleParameter2D, -260, 60)
    samp.set_editor_property("parameter_name", "RibbonTex")
    if tex:
        samp.set_editor_property("texture", tex)
    else:
        unreal.log_warning("未找到默认贴图, 请在材质里手动指定 RibbonTex")
    connect(mel, spd_add, ["", "Output"], samp, ["UVs", "Coordinates", "Coordinate"])

    # --- 颜色链: 贴图RGB × Tint × 顶点色 → 自发光 ---
    tint = ex(unreal.MaterialExpressionVectorParameter, -260, 420)
    tint.set_editor_property("parameter_name", "Tint")
    tint.set_editor_property("default_r", 1.0)
    tint.set_editor_property("default_g", 0.16)
    tint.set_editor_property("default_b", 0.12)
    tint.set_editor_property("default_a", 1.0)
    vc = ex(unreal.MaterialExpressionVertexColor, -260, 680)
    mul_rgb1 = ex(unreal.MaterialExpressionMultiply, 100, 240)
    mul_rgb2 = ex(unreal.MaterialExpressionMultiply, 420, 340)
    connect(mel, samp, ["RGB", "Color"], mul_rgb1, ["A"])
    connect(mel, tint, ["RGB", ""], mul_rgb1, ["B"])
    connect(mel, mul_rgb1, ["", "Output"], mul_rgb2, ["A"])
    connect(mel, vc, ["Color", ""], mul_rgb2, ["B"])
    mel.connect_material_property(mul_rgb2, "", unreal.MaterialProperty.MP_EMISSIVECOLOR)

    # --- 透明链: 贴图A × 顶点色A → 不透明度 ---
    mul_a = ex(unreal.MaterialExpressionMultiply, 420, 640)
    connect(mel, samp, ["A"], mul_a, ["A"])
    connect(mel, vc, ["A"], mul_a, ["B"])
    mel.connect_material_property(mul_a, "", unreal.MaterialProperty.MP_OPACITY)

    eal.save_loaded_asset(mat)
    print("material saved:", pkg)


main()
