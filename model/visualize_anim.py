"""
Animated GIF 3D Pareto front visualizer.

Usage:
    python3 visualize_anim.py <path_to_viz_data.txt> [output.gif]
    python3 visualize_anim.py                         # auto-searches results/ for merged or *_viz_data.txt

Output defaults to pareto_animation.gif in the current directory.

Requirements:
    pip install matplotlib pillow
"""

import sys
import os
import glob
import math

import matplotlib
matplotlib.use("Agg")  # non-interactive backend required for GIF generation
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from mpl_toolkits.mplot3d import Axes3D  # noqa: F401 — registers 3D projection


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
                    "source": parts[10].strip() if len(parts) > 10 else "default",
                })
    return ideal, solutions


def find_data_file() -> str:
    """Auto-locate a viz data file in results/: prefer merged, then most recent."""
    merged = glob.glob("results/merged_viz_data.txt")
    if merged:
        return merged[0]
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


# ── Color palette ─────────────────────────────────────────────────────────────

_COLORS = [
    "royalblue", "orangered", "seagreen", "darkorchid", "goldenrod",
    "deeppink",  "darkcyan",  "saddlebrown", "slategray", "limegreen",
]


# ── Animation builder ─────────────────────────────────────────────────────────

def build_animation(ideal, solutions, output_path="pareto_animation.gif",
                    n_frames=72, fps=15, elev=25):
    """
    Render both Pareto subplots and save a rotating animation as a GIF.

    Parameters
    ----------
    ideal        : (A, B, C) tuple
    solutions    : list of solution dicts
    output_path  : destination GIF file
    n_frames     : total frames in one full 360° rotation
    fps          : frames per second
    elev         : fixed camera elevation angle (degrees)
    """
    sources = list(dict.fromkeys(s["source"] for s in solutions))
    color_map = {src: _COLORS[i % len(_COLORS)] for i, src in enumerate(sources)}

    fig = plt.figure(figsize=(16, 7), facecolor="white")
    fig.suptitle("Multi-Criteria Optimization — Pareto Front",
                 fontsize=14, fontweight="bold", y=0.98)

    ax1 = fig.add_subplot(121, projection="3d")
    ax2 = fig.add_subplot(122, projection="3d")

    for ax in (ax1, ax2):
        ax.set_facecolor("#f5f5ff")
        ax.xaxis.pane.fill = True
        ax.yaxis.pane.fill = True
        ax.zaxis.pane.fill = True
        ax.xaxis.pane.set_facecolor("#ececf8")
        ax.yaxis.pane.set_facecolor("#ececf8")
        ax.zaxis.pane.set_facecolor("#ececf8")
        ax.grid(True, color="white", linewidth=0.8)

    # ── Raw values subplot ────────────────────────────────────────────────────
    for src in sources:
        src_sols = [s for s in solutions if s["source"] == src]
        ax1.scatter(
            [s["A"] for s in src_sols],
            [s["B"] for s in src_sols],
            [s["C"] for s in src_sols],
            c=color_map[src], s=55, alpha=0.85,
            edgecolors="white", linewidths=0.5,
            label=src, depthshade=True,
        )

    ax1.scatter(
        [ideal[0]], [ideal[1]], [ideal[2]],
        c="red", s=180, marker="D",
        edgecolors="darkred", linewidths=1.5,
        label="Ideal point", depthshade=False, zorder=10,
    )

    ax1.set_xlabel("A (physical fitness)", labelpad=8, fontsize=9)
    ax1.set_ylabel("B (individual skills)", labelpad=8, fontsize=9)
    ax1.set_zlabel("C (team tactics)", labelpad=8, fontsize=9)
    ax1.set_title("Raw values (A, B, C)", fontsize=11, pad=12)

    rA = axis_range([s["A"] for s in solutions] + [ideal[0]])
    rB = axis_range([s["B"] for s in solutions] + [ideal[1]])
    rC = axis_range([s["C"] for s in solutions] + [ideal[2]])
    ax1.set_xlim(rA)
    ax1.set_ylim(rB)
    ax1.set_zlim(rC)

    ax1.legend(loc="upper left", fontsize=8,
               framealpha=0.7, edgecolor="lightgrey")

    # ── Normalized values subplot ─────────────────────────────────────────────
    for src in sources:
        src_sols = [s for s in solutions if s["source"] == src]
        ax2.scatter(
            [s["A_norm"] for s in src_sols],
            [s["B_norm"] for s in src_sols],
            [s["C_norm"] for s in src_sols],
            c=color_map[src], s=55, alpha=0.85,
            edgecolors="white", linewidths=0.5,
            depthshade=True,
        )

    ax2.scatter(
        [1.0], [1.0], [1.0],
        c="red", s=180, marker="D",
        edgecolors="darkred", linewidths=1.5,
        depthshade=False, zorder=10,
    )

    ax2.set_xlabel("A / A*", labelpad=8, fontsize=9)
    ax2.set_ylabel("B / B*", labelpad=8, fontsize=9)
    ax2.set_zlabel("C / C*", labelpad=8, fontsize=9)
    ax2.set_title("Normalized (A/A*, B/B*, C/C*)", fontsize=11, pad=12)
    ax2.set_xlim([0.0, 1.0])
    ax2.set_ylim([0.0, 1.0])
    ax2.set_zlim([0.0, 1.0])
    ax2.set_box_aspect([1, 1, 1])

    plt.tight_layout(rect=[0, 0, 1, 0.95])

    # ── Animation update: rotate azimuth 0→360° ───────────────────────────────
    def update(frame):
        azim = frame * 360.0 / n_frames
        ax1.view_init(elev=elev, azim=azim)
        ax2.view_init(elev=elev, azim=azim)
        return (fig,)

    anim = animation.FuncAnimation(
        fig, update, frames=n_frames,
        interval=1000 // fps, blit=False,
    )

    print(f"Rendering {n_frames} frames → {output_path} ...")
    writer = animation.PillowWriter(fps=fps)
    anim.save(output_path, writer=writer, dpi=100)
    plt.close(fig)
    print(f"Saved: {output_path}")


# ── Entry point ───────────────────────────────────────────────────────────────

def main():
    args = sys.argv[1:]

    # Parse: [data_path] [output.gif]  (order-independent for .gif argument)
    data_path = None
    output_path = "pareto_animation.gif"

    for arg in args:
        if arg.lower().endswith(".gif"):
            output_path = arg
        else:
            data_path = arg

    if data_path is None:
        data_path = find_data_file()

    if not os.path.exists(data_path):
        sys.exit(f"File not found: {data_path}")

    print(f"Loading: {data_path}")
    ideal, solutions = load_viz_data(data_path)

    if ideal is None:
        sys.exit("No ideal_point line found in the data file.")
    if not solutions:
        sys.exit("No solution lines found in the data file.")

    print(f"  Ideal point : A={ideal[0]:.4f}  B={ideal[1]:.4f}  C={ideal[2]:.4f}")
    print(f"  Solutions   : {len(solutions)}")

    sources = list(dict.fromkeys(s["source"] for s in solutions))
    for src in sources:
        n = sum(1 for s in solutions if s["source"] == src)
        print(f"  Source '{src}': {n} solutions")

    build_animation(ideal, solutions, output_path)


if __name__ == "__main__":
    main()
