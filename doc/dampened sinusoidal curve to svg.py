import math

N = 100
xmin, xmax = -1.6, 1.6

x0, x1 = 50, 750
y0, y1 = 250, 50

def f(x):
    c = math.cos(math.pi*x/3.2)
    return math.cos(2*math.pi*x) * math.copysign(abs(c)**1.22, c)

pts = []
for i in range(N):
    t = i / (N - 1)
    x = xmin + t * (xmax - xmin)
    y = f(x)

    X = x0 + (x - xmin) / (xmax - xmin) * (x1 - x0)
    Y = y0 + (1 - (y + 1) / 2) * (y0 - y1)
    pts.append((X, Y))

d = f"M {pts[0][0]:.3f} {pts[0][1]:.3f}"

for i in range(1, len(pts) - 1):
    p0 = pts[i - 1]
    p1 = pts[i]
    p2 = pts[i + 1]
    p3 = pts[i + 2] if i + 2 < len(pts) else p2

    c1x = p1[0] + (p2[0] - p0[0]) / 6
    c1y = p1[1] + (p2[1] - p0[1]) / 6
    c2x = p2[0] - (p3[0] - p1[0]) / 6
    c2y = p2[1] - (p3[1] - p1[1]) / 6

    d += f" C {c1x:.3f} {c1y:.3f}, {c2x:.3f} {c2y:.3f}, {p2[0]:.3f} {p2[1]:.3f}"

print(f"""<svg xmlns="http://www.w3.org/2000/svg" width="800" height="300">
  <path d="{d}"
        fill="none"
        stroke="black"
        stroke-width="2"/>
</svg>""")
