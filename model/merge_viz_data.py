"""
Merge *_viz_data.txt and *_subtasks_viz_data.txt files from results/ into a
single file that visualize.py uses to color solutions by source.

Usage:
    python3 merge_viz_data.py [output_path]

    output_path defaults to results/merged_viz_data.txt

Output format:
    ideal_point,A,B,C
    solution,wA,wB,wC,A,B,C,A_norm,B_norm,C_norm,source_label
"""

import sys
import os
import glob


def merge_viz_data(output_path: str = "results/merged_viz_data.txt"):
    # Collect subtask files first, then regular files (so source labels are deterministic)
    subtask_files = sorted(glob.glob("results/*_subtasks_viz_data.txt"))
    regular_files = sorted(
        f for f in glob.glob("results/*_viz_data.txt")
        if "_subtasks_viz_data" not in f
        and os.path.basename(f) != "merged_viz_data.txt"
    )

    all_files = []
    for f in regular_files:
        base = os.path.basename(f).replace("_viz_data.txt", "")
        all_files.append((f, base))
    for f in subtask_files:
        base = os.path.basename(f).replace("_subtasks_viz_data.txt", "") + "_subtasks"
        all_files.append((f, base))

    if not all_files:
        print("No *_viz_data.txt files found in results/")
        return

    ideal_point = None
    all_solutions = []

    for file_path, source_label in all_files:
        n_before = len(all_solutions)
        with open(file_path) as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith("#"):
                    continue
                parts = line.split(",")
                if parts[0] == "ideal_point" and ideal_point is None:
                    ideal_point = (float(parts[1]), float(parts[2]), float(parts[3]))
                elif parts[0] == "solution":
                    all_solutions.append((source_label, parts[1:]))
        print(f"  {file_path}: {len(all_solutions) - n_before} solutions (source='{source_label}')")

    if ideal_point is None:
        print("Warning: no ideal_point found in any file — normalization will be missing")

    os.makedirs(os.path.dirname(output_path) or ".", exist_ok=True)
    with open(output_path, "w") as out:
        out.write("# Merged multi-criteria optimization visualization data\n")
        out.write("# ideal_point,A,B,C\n")
        out.write("# solution,wA,wB,wC,A,B,C,A_norm,B_norm,C_norm,source\n")
        if ideal_point is not None:
            out.write(f"ideal_point,{ideal_point[0]},{ideal_point[1]},{ideal_point[2]}\n")
        for source_label, parts in all_solutions:
            out.write(f"solution,{','.join(parts)},{source_label}\n")

    print(f"Merged {len(all_solutions)} solutions from {len(all_files)} file(s) → {output_path}")


if __name__ == "__main__":
    output_path = sys.argv[1] if len(sys.argv) > 1 else "results/merged_viz_data.txt"
    merge_viz_data(output_path)
