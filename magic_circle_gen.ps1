# Magic circle PNG generator (white strokes, transparent background) v3
# NOTE: keep this file pure ASCII! PS 5.1 reads BOM-less files as GBK on zh-CN,
# and certain UTF-8 Chinese comments can swallow the NEXT line via byte-pairing.
# Usage: powershell -ExecutionPolicy Bypass -File magic_circle_gen.ps1 [OutPath] [Size]
param(
    [string]$OutPath = "D:\UnReal5\NiagarasDemo\Content\Code\Day1\Textures\MagicCircle.png",
    [int]$Size = 2048
)

Add-Type -AssemblyName System.Drawing

$k    = $Size / 2048.0
$cc   = $Size / 2.0
$Thick = 1.45   # global stroke width multiplier (1.0 = original)
$rnd  = [System.Random]::new(20260831)

$bmp = New-Object System.Drawing.Bitmap($Size, $Size, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
$g   = [System.Drawing.Graphics]::FromImage($bmp)
$g.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::AntiAlias
$g.Clear([System.Drawing.Color]::Transparent)

function New-Pen([double]$w, [int]$a = 255) {
    New-Object System.Drawing.Pen(([System.Drawing.Color]::FromArgb($a, 255, 255, 255)), [float]($w * $k * $Thick))
}
function New-Pt([double]$x, [double]$y) {
    New-Object System.Drawing.PointF([float]($x * $k), [float]($y * $k))
}
function Draw-Circle([double]$r, [double]$w, [int]$a = 255) {
    $p = New-Pen $w $a
    $g.DrawEllipse($p, [float]($cc - $r * $k), [float]($cc - $r * $k), [float](2 * $r * $k), [float](2 * $r * $k))
    $p.Dispose()
}
function Pt([double]$radius, [double]$deg) {
    $rad = $deg * [Math]::PI / 180.0
    New-Pt ($cc / $k + $radius * [Math]::Cos($rad)) ($cc / $k - $radius * [Math]::Sin($rad))
}

$brush = New-Object System.Drawing.SolidBrush([System.Drawing.Color]::FromArgb(255, 255, 255, 255))

# ---- 1. outer double ring ----
Draw-Circle 1000 14
Draw-Circle 925 5.5

# ---- 2. rune band ----
$runePen = New-Pen 4
for ($i = 0; $i -lt 28; $i++) {
    $g.TranslateTransform([float]$cc, [float]$cc)
    $g.RotateTransform([float]($i * 360.0 / 28.0))
    $g.TranslateTransform(0.0, [float](-962.5 * $k))
    $g.DrawLine($runePen, [float]0, [float](-26 * $k), [float]0, [float](26 * $k))
    $branches = 2 + $rnd.Next(0, 2)
    for ($b = 0; $b -lt $branches; $b++) {
        $y0  = (-26 + $rnd.Next(0, 47)) * $k
        $dir = @{ $true = 1; $false = -1 }[($rnd.Next(0, 2) -eq 1)]
        $y1  = $y0 + (10 + $rnd.Next(0, 18)) * $k
        $g.DrawLine($runePen, [float]0, [float]$y0, [float]($dir * 14 * $k), [float]$y1)
    }
    $g.ResetTransform()
}
$runePen.Dispose()

# ---- 3. tick ring ----
for ($i = 0; $i -lt 96; $i++) {
    $a     = $i * 3.75
    $major = ($i % 8 -eq 0)
    $rIn   = @{ $true = 875.0; $false = 890.0 }[$major]
    $pen   = New-Pen $(@{ $true = 4.5; $false = 2.5 }[$major]) $(@{ $true = 255; $false = 210 }[$major])
    $p1 = Pt $rIn $a; $p2 = Pt 910 $a
    $g.DrawLine($pen, $p1.X, $p1.Y, $p2.X, $p2.Y)
    $pen.Dispose()
}

# ---- 4. dashed arc ring ----
$arcPen = New-Pen 3 210
for ($i = 0; $i -lt 12; $i++) {
    $g.DrawArc($arcPen, [float]($cc - 840 * $k), [float]($cc - 840 * $k),
        [float](1680 * $k), [float](1680 * $k), [float]($i * 30.0 + 4), 16.0)
}
$arcPen.Dispose()

# ---- 5. hexagram + circumscribed circle ----
$hexPen = New-Pen 3.5
$tri1 = @((Pt 790 90), (Pt 790 210), (Pt 790 330))
$tri2 = @((Pt 790 30), (Pt 790 150), (Pt 790 270))
$g.DrawPolygon($hexPen, [System.Drawing.PointF[]]$tri1)
$g.DrawPolygon($hexPen, [System.Drawing.PointF[]]$tri2)
Draw-Circle 790 4
$hexPen.Dispose()

# ---- 6. vertex circles + dots ----
$vPen = New-Pen 3
foreach ($deg in @(30, 90, 150, 210, 270, 330)) {
    $pt = Pt 790 $deg
    $g.DrawEllipse($vPen, [float]($pt.X - 27.5 * $k), [float]($pt.Y - 27.5 * $k), [float](55 * $k), [float](55 * $k))
    $g.FillEllipse($brush, [float]($pt.X - 10.5 * $k), [float]($pt.Y - 10.5 * $k), [float](21 * $k), [float](21 * $k))
}
$vPen.Dispose()

# ---- 7. inner rings ----
Draw-Circle 395 6.5
Draw-Circle 362.5 2 200

# ---- 8. center: circle + pentagram + dot ----
Draw-Circle 155 3.5
$starPen = New-Pen 3
$pts = @()
for ($i = 0; $i -lt 10; $i++) {
    $rr   = @{ $true = 140.0; $false = 140.0 * 0.382 }[($i % 2 -eq 0)]
    $pts += (Pt $rr (90.0 + $i * 36.0))
}
$g.DrawPolygon($starPen, [System.Drawing.PointF[]]$pts)
$starPen.Dispose()
Draw-Circle 65 2 210
$g.FillEllipse($brush, [float]($cc - 13.5 * $k), [float]($cc - 13.5 * $k), [float](27 * $k), [float](27 * $k))
$brush.Dispose()

# ---- save ----
$dir = Split-Path $OutPath -Parent
if (-not (Test-Path $dir)) { New-Item -ItemType Directory -Path $dir -Force | Out-Null }
$g.Dispose()
$bmp.Save($OutPath, [System.Drawing.Imaging.ImageFormat]::Png)
$bmp.Dispose()

# ---- self check: 8-direction ring + center opaque, corner transparent ----
$chk = New-Object System.Drawing.Bitmap($OutPath)
$mid = $Size / 2
$fail = @()
foreach ($a in @(0, 45, 90, 135, 180, 225, 270, 315)) {
    $rad = $a * [Math]::PI / 180.0
    $best = 0
    for ($dr = -12; $dr -le 12; $dr += 2) {
        $r  = 1000.0 * $k + $dr * $k
        $px = [int]($mid + $r * [Math]::Cos($rad)); $py = [int]($mid - $r * [Math]::Sin($rad))
        if ($px -ge 0 -and $px -lt $Size -and $py -ge 0 -and $py -lt $Size) {
            $best = [Math]::Max($best, $chk.GetPixel($px, $py).A)
        }
    }
    if ($best -lt 100) { $fail += "ring@$a" }
}
if ($chk.GetPixel([int]$mid, [int]$mid).A -lt 200) { $fail += "center" }
if ($chk.GetPixel(2, 2).A -ne 0) { $fail += "corner" }

if ($fail.Count -eq 0) {
    Write-Host "SELF-CHECK PASS: 8-direction ring OK, center OK, corner transparent."
    Write-Host "Saved: $OutPath ($Size x $Size)"
} else {
    Write-Host ("SELF-CHECK FAIL: " + ($fail -join ", "))
}
$chk.Dispose()
