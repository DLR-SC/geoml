"""Generate the geoml logo SVGs.

Usage: python images/make_logo.py [output directory]
Writes geoml-logo.svg (light backgrounds), geoml-logo-white.svg (dark
backgrounds, used in the docs sidebar) and geoml-icon.svg (mark only, favicon).


Mark: a wing segment lofted through a network of profile and guide curves,
i.e. a surface interpolating a curve network - the core idea of geoml.
Wordmark: "geoml" constructed from circles, arcs and lines (no font needed).
"""
import math
import pathlib
import sys

# output directory, default: docs/_static next to this script
OUT = (pathlib.Path(sys.argv[1]) if len(sys.argv) > 1
       else pathlib.Path(__file__).resolve().parent.parent / "docs" / "_static")
OUT.mkdir(parents=True, exist_ok=True)

BLUE_DARK = "#1d4560"
BLUE = "#24506f"
BLUE_LIGHT = "#3b7aa3"
ORANGE = "#e9821c"
ORANGE_LIGHT = "#f59a3c"


# ----------------------------------------------------------------- geometry
# The mark: a wing segment lofted through airfoil profiles, drawn in an
# oblique projection inside a 128 x 128 box. Orange: the curve network of
# profiles (span direction) and guide curves (leading/trailing edge), i.e. the
# input of a Gordon surface. Blue: the upper skin interpolating the network.

def bezier(p0, p1, p2, p3, t):
    s = 1 - t
    return tuple(s**3 * a + 3 * s * s * t * b + 3 * s * t * t * c + t**3 * d
                 for a, b, c, d in zip(p0, p1, p2, p3))


# airfoil in section coordinates (c: 0 = trailing edge .. 1 = leading edge,
# t: thickness, positive = upper side); nose at (1, 0) with vertical tangent
UPPER = ((0.0, 0.0), (0.40, 0.11), (0.80, 0.17), (1.0, 0.02))
LOWER = ((0.0, 0.0), (0.40, -0.03), (0.80, -0.13), (1.0, 0.02))

ORIGIN = (6.0, 90.0)       # trailing edge of the tip profile
CHORD = (54.0, 18.0)        # tip chord vector on screen (TE -> LE)
THICK = (0.0, -104.0)        # thickness direction on screen per unit chord
SPAN = (68.0, -74.0)        # span vector on screen (tip -> root)
TAPER = 0.56                # root chord / tip chord


def wing_point(s, c, t):
    """Screen point at span s in [0, 1] and section coordinates (c, t)."""
    k = 1 - (1 - TAPER) * s
    return tuple(ORIGIN[i] + s * SPAN[i] + k * (c * CHORD[i] + t * THICK[i]) for i in range(2))


def section(curve, s, n=40, reverse=False):
    pts = [wing_point(s, *bezier(*curve, k / n)) for k in range(n + 1)]
    return pts[::-1] if reverse else pts


def guide(c, t_curve=None, n=24):
    """Curve in span direction at chord position c on the upper side."""
    t = bezier(*UPPER, c)[1] if t_curve is None else t_curve
    return [wing_point(k / n, c, t) for k in range(n + 1)]


def polyline(points, close=False):
    return "M" + " L".join(f"{x:.2f},{y:.2f}" for x, y in points) + (" Z" if close else "")


def mark(prefix, tx=0.0, ty=0.0, scale=1.0, network=ORANGE, face="url(#{p}-face)"):
    face = face.format(p=prefix)
    tip_upper = section(UPPER, 0)
    skin = tip_upper + guide(1.0, 0.02)[1:] + section(UPPER, 1, reverse=True)[1:] + guide(0.0, 0.0)[::-1][1:]
    tip_closed = section(UPPER, 0) + section(LOWER, 0, reverse=True)[1:]

    # interior network curves on the skin
    profiles = [section(UPPER, s) for s in (0.5, 1.0)]
    guides = [guide(0.0, 0.0), guide(1.0, 0.02)]
    dots = [wing_point(s, c, t) for s in (0.5, 1.0) for c, t in ((0.0, 0.0), (1.0, 0.02))]
    # the tip profile cap: a vertical curve splitting the section face
    tip_mid = [wing_point(0, *bezier(*UPPER, 0.62)), wing_point(0, *bezier(*LOWER, 0.62))]

    # fit the drawing into the 128 x 128 box with a margin
    allpts = skin + tip_closed
    x0, x1 = min(p[0] for p in allpts), max(p[0] for p in allpts)
    y0, y1 = min(p[1] for p in allpts), max(p[1] for p in allpts)
    margin = 6
    f = (128 - 2 * margin) / max(x1 - x0, y1 - y0)
    fx = margin + ((128 - 2 * margin) - f * (x1 - x0)) / 2 - f * x0
    fy = margin + ((128 - 2 * margin) - f * (y1 - y0)) / 2 - f * y0

    sw = 3.0 / f
    parts = [f'<g transform="translate({tx} {ty}) scale({scale})">',
             f'<g transform="translate({fx:.3f} {fy:.3f}) scale({f:.4f})">',
             f'<path d="{polyline(skin, True)}" fill="url(#{prefix}-skin)"/>',
             f'<path d="{polyline(skin, True)}" fill="url(#{prefix}-sheen)"/>']
    for pts in profiles + guides:
        parts.append(f'<path d="{polyline(pts)}" fill="none" stroke="{network}" stroke-width="{sw}" '
                     f'stroke-linecap="round" stroke-linejoin="round"/>')
    parts.append(f'<path d="{polyline(tip_closed, True)}" fill="{face}" stroke="{network}" '
                 f'stroke-width="{sw + 0.8}" stroke-linejoin="round"/>')
    parts.append(f'<path d="{polyline(tip_mid)}" fill="none" stroke="{network}" stroke-width="{sw - 0.6}" '
                 f'stroke-linecap="round"/>')
    for x, y in dots:
        parts.append(f'<circle cx="{x:.2f}" cy="{y:.2f}" r="{3.4 / f:.2f}" fill="#ffffff" stroke="{network}" stroke-width="{1.6 / f:.2f}"/>')
    parts.append("</g></g>")
    return "\n".join(parts)


def mark_defs(prefix):
    return f"""<linearGradient id="{prefix}-skin" x1="0" y1="1" x2="1" y2="0">
  <stop offset="0" stop-color="{BLUE_DARK}"/>
  <stop offset="0.5" stop-color="{BLUE}"/>
  <stop offset="1" stop-color="{BLUE_LIGHT}"/>
</linearGradient>
<linearGradient id="{prefix}-sheen" x1="0" y1="0" x2="1" y2="1">
  <stop offset="0.30" stop-color="#ffffff" stop-opacity="0"/>
  <stop offset="0.50" stop-color="#ffffff" stop-opacity="0.18"/>
  <stop offset="0.66" stop-color="#ffffff" stop-opacity="0"/>
</linearGradient>
<linearGradient id="{prefix}-face" x1="0" y1="0" x2="0" y2="1">
  <stop offset="0" stop-color="#ffffff"/>
  <stop offset="1" stop-color="#dfe6ea"/>
</linearGradient>"""


# ----------------------------------------------------------------- wordmark
# Monoline letters: x-height from y=40 to y=80, stroke width SW, bowl radius R
SW = 9.0
R = 20 - SW / 2          # centerline radius of the round letters
TOP, MID, BASE = 40 + SW / 2, 60, 80 - SW / 2


def arc_point(cx, cy, r, deg):
    a = math.radians(deg)
    return cx + r * math.cos(a), cy - r * math.sin(a)


def letter_g(x):
    cx = x + 20
    sx = cx + R
    hook_r = 13
    return [
        f"M{cx + R:.2f},{MID} A{R},{R} 0 1 0 {cx - R:.2f},{MID} A{R},{R} 0 1 0 {cx + R:.2f},{MID}",  # bowl
        f"M{sx:.2f},{TOP} V{BASE + 12:.2f} "
        f"A{hook_r},{hook_r} 0 0 1 {sx - hook_r:.2f},{BASE + 12 + hook_r:.2f} "
        f"H{cx - 6:.2f}",
    ], 40


def letter_e(x):
    cx = x + 20
    end = arc_point(cx, MID, R, -42)
    return [
        f"M{cx - R:.2f},{MID} H{cx + R:.2f} A{R},{R} 0 1 0 {end[0]:.2f},{end[1]:.2f}",
    ], 40


def letter_o(x):
    cx = x + 20
    return [f"M{cx + R:.2f},{MID} A{R},{R} 0 1 0 {cx - R:.2f},{MID} A{R},{R} 0 1 0 {cx + R:.2f},{MID}"], 40


def letter_m(x):
    a = 11.5                 # arch radius
    x0 = x + SW / 2
    x1 = x0 + 2 * a
    x2 = x1 + 2 * a
    top = TOP + a
    return [
        f"M{x0:.2f},{BASE} V{top:.2f} A{a},{a} 0 0 1 {x1:.2f},{top:.2f} V{BASE} "
        f"M{x1:.2f},{top:.2f} A{a},{a} 0 0 1 {x2:.2f},{top:.2f} V{BASE}",
    ], 4 * a + SW


def letter_l(x):
    x0 = x + SW / 2
    return [f"M{x0:.2f},{18 + SW / 2} V{BASE - 6:.2f} A6,6 0 0 0 {x0 + 6:.2f},{BASE}"], SW + 6


def wordmark(x, color_geo, color_ml, gap=7):
    paths = []
    for letter, color in ((letter_g, color_geo), (letter_e, color_geo), (letter_o, color_geo),
                          (letter_m, color_ml), (letter_l, color_ml)):
        ds, width = letter(x)
        for d in ds:
            paths.append(f'<path d="{d}" fill="none" stroke="{color}" stroke-width="{SW}" '
                         f'stroke-linecap="round" stroke-linejoin="round"/>')
        x += width + gap
    return "\n".join(paths), x - gap


# ----------------------------------------------------------------- files
def svg(width, height, body, defs="", title="geoml"):
    return (f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {width:.0f} {height:.0f}" '
            f'width="{width:.0f}" height="{height:.0f}" role="img" aria-label="{title}">\n'
            f"<title>{title}</title>\n<defs>\n{defs}\n</defs>\n{body}\n</svg>\n")


def write(name, content):
    (OUT / name).write_text(content, encoding="utf-8")
    print("wrote", OUT / name)


# icon: mark only
write("geoml-icon.svg", svg(128, 128, mark("i"), mark_defs("i"), "geoml icon"))

# horizontal logo: mark scaled to the wordmark height + wordmark
MARK_SCALE = 0.78
mark_w = 128 * MARK_SCALE
text_x = mark_w + 4
for name, geo, ml, network in (
    ("geoml-logo.svg", BLUE, ORANGE, ORANGE),
    ("geoml-logo-white.svg", "#ffffff", ORANGE_LIGHT, ORANGE_LIGHT),
):
    words, end_x = wordmark(text_x, geo, ml)
    body = mark("m", 0, 3, MARK_SCALE, network=network) + "\n" + words
    write(name, svg(end_x + 6, 108, body, mark_defs("m"), "geoml"))
