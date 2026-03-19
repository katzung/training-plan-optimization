"""
Interactive 3D Pareto front visualizer.

Usage:
    python3 visualize.py <path_to_viz_data.txt>
    python3 visualize.py                         # auto-searches results/ for *_viz_data.txt

Requirements:
    pip install plotly
"""

import sys
import os
import glob
import math

import plotly.graph_objects as go
from plotly.subplots import make_subplots


# ── Data loading ──────────────────────────────────────────────────────────────

def load_viz_data(path: str):
    ideal = None
    solutions = []
    with open(path) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            parts = line.split(",")
            if parts[0] == "ideal_point":
                ideal = (float(parts[1]), float(parts[2]), float(parts[3]))
            elif parts[0] == "solution":
                solutions.append({
                    "wA":     float(parts[1]),
                    "wB":     float(parts[2]),
                    "wC":     float(parts[3]),
                    "A":      float(parts[4]),
                    "B":      float(parts[5]),
                    "C":      float(parts[6]),
                    "A_norm": float(parts[7]),
                    "B_norm": float(parts[8]),
                    "C_norm": float(parts[9]),
                })
    return ideal, solutions


def find_data_file() -> str:
    """Auto-locate the most recently modified *_viz_data.txt in results/."""
    candidates = glob.glob("results/*_viz_data.txt")
    if not candidates:
        sys.exit("No *_viz_data.txt found in results/. "
                 "Run the Julia notebook first, or pass the file path as an argument.")
    return max(candidates, key=os.path.getmtime)


# ── Helpers ───────────────────────────────────────────────────────────────────

def axis_range(values, padding_frac=0.2, min_padding=1e-3):
    """Return [lo, hi] with padding so tightly-clustered points stay visible."""
    clean = [v for v in values if v is not None and not math.isnan(v) and not math.isinf(v)]
    if not clean:
        return [0.0, 1.0]
    lo, hi = min(clean), max(clean)
    pad = max((hi - lo) * padding_frac, min_padding)
    return [lo - pad, hi + pad]


def _hover(s):
    return (
        f"w = ({s['wA']:.2f}, {s['wB']:.2f}, {s['wC']:.2f})<br>"
        f"A = {s['A']:.4f}<br>B = {s['B']:.4f}<br>C = {s['C']:.4f}"
    )


def _hover_norm(s):
    return (
        f"w = ({s['wA']:.2f}, {s['wB']:.2f}, {s['wC']:.2f})<br>"
        f"A/A* = {s['A_norm']:.4f}<br>B/B* = {s['B_norm']:.4f}<br>C/C* = {s['C_norm']:.4f}"
    )


# ── Plot builder ──────────────────────────────────────────────────────────────

def build_figure(ideal, solutions):
    fig = make_subplots(
        rows=1, cols=2,
        specs=[[{"type": "scene"}, {"type": "scene"}]],
        subplot_titles=["Raw values (A, B, C)", "Normalized (A/A*, B/B*, C/C*)"],
        horizontal_spacing=0.05,
    )

    scene_axes = dict(
        backgroundcolor="rgb(245,245,255)",
        gridcolor="white",
        showbackground=True,
        zerolinecolor="white",
    )

    # ── Raw values ─────────────────────────────────────────────────────────
    A_vals = [s["A"] for s in solutions]
    B_vals = [s["B"] for s in solutions]
    C_vals = [s["C"] for s in solutions]

    fig.add_trace(go.Scatter3d(
        x=A_vals, y=B_vals, z=C_vals,
        mode="markers",
        marker=dict(color="royalblue", size=8, opacity=0.85,
                    line=dict(color="white", width=1)),
        name="Solutions",
        hovertemplate="%{customdata}<extra></extra>",
        customdata=[_hover(s) for s in solutions],
    ), row=1, col=1)

    fig.add_trace(go.Scatter3d(
        x=[ideal[0]], y=[ideal[1]], z=[ideal[2]],
        mode="markers+text",
        marker=dict(color="red", size=14, symbol="diamond",
                    line=dict(color="darkred", width=2)),
        text=["Ideal"], textposition="top center",
        textfont=dict(color="red", size=12),
        name="Ideal point",
        hovertemplate=(
            f"Ideal point<br>A={ideal[0]:.4f}<br>"
            f"B={ideal[1]:.4f}<br>C={ideal[2]:.4f}<extra></extra>"
        ),
    ), row=1, col=1)

    # ── Normalized values ──────────────────────────────────────────────────
    An_vals = [s["A_norm"] for s in solutions]
    Bn_vals = [s["B_norm"] for s in solutions]
    Cn_vals = [s["C_norm"] for s in solutions]

    fig.add_trace(go.Scatter3d(
        x=An_vals, y=Bn_vals, z=Cn_vals,
        mode="markers",
        marker=dict(color="royalblue", size=8, opacity=0.85,
                    line=dict(color="white", width=1)),
        name="Solutions (norm)",
        showlegend=False,
        hovertemplate="%{customdata}<extra></extra>",
        customdata=[_hover_norm(s) for s in solutions],
        scene="scene2",
    ))

    fig.add_trace(go.Scatter3d(
        x=[1.0], y=[1.0], z=[1.0],
        mode="markers+text",
        marker=dict(color="red", size=14, symbol="diamond",
                    line=dict(color="darkred", width=2)),
        text=["Ideal"], textposition="top center",
        textfont=dict(color="red", size=12),
        name="Ideal point (norm)",
        showlegend=False,
        hovertemplate="Ideal point<br>A/A*=1.0<br>B/B*=1.0<br>C/C*=1.0<extra></extra>",
        scene="scene2",
    ))

    # ── Layout: explicit axis ranges to handle tightly-clustered data ──────
    fig.update_layout(
        title=dict(text="Multi-Criteria Optimization — Pareto Front",
                   font=dict(size=20), x=0.5),
        scene=dict(
            xaxis=dict(**scene_axes, title="A (physical fitness)"),
            yaxis=dict(**scene_axes, title="B (individual skills)"),
            zaxis=dict(**scene_axes, title="C (team tactics)"),
        ),
        scene2=dict(
            xaxis=dict(**scene_axes, title="A / A*",
                       range=axis_range(An_vals + [1.0])),
            yaxis=dict(**scene_axes, title="B / B*",
                       range=axis_range(Bn_vals + [1.0])),
            zaxis=dict(**scene_axes, title="C / C*",
                       range=axis_range(Cn_vals + [1.0])),
            aspectmode="cube",
        ),
        legend=dict(x=0.01, y=0.99, bgcolor="rgba(255,255,255,0.7)",
                    bordercolor="lightgrey", borderwidth=1),
        margin=dict(l=0, r=0, t=60, b=0),
        paper_bgcolor="white",
    )

    return fig


# ── Entry point ───────────────────────────────────────────────────────────────

def main():
    if len(sys.argv) > 1:
        data_path = sys.argv[1]
        if not os.path.exists(data_path):
            sys.exit(f"File not found: {data_path}")
    else:
        data_path = find_data_file()

    print(f"Loading: {data_path}")
    ideal, solutions = load_viz_data(data_path)

    if ideal is None:
        sys.exit("No ideal_point line found in the data file.")
    if not solutions:
        sys.exit("No solution lines found in the data file.")

    print(f"  Ideal point : A={ideal[0]:.4f}  B={ideal[1]:.4f}  C={ideal[2]:.4f}")
    print(f"  Solutions   : {len(solutions)}")

    for i, s in enumerate(solutions):
        print(f"  Solution {i+1} : "
              f"A={s['A']:.4f} B={s['B']:.4f} C={s['C']:.4f} | "
              f"A_norm={s['A_norm']:.4f} B_norm={s['B_norm']:.4f} C_norm={s['C_norm']:.4f}")

    fig = build_figure(ideal, solutions)
    fig.show()


if __name__ == "__main__":
    main()
