#!/usr/bin/env python3
"""
Coherent Point Drift (CPD) registration script for CustusX.

Usage:
    python3 cpd_registration.py <fixed_pts> <moving_pts> <result_pts> <reg_type> <max_iter> <tolerance>

Arguments:
    fixed_pts   Path to fixed point cloud file (one "x y z" per line)
    moving_pts  Path to moving point cloud file (one "x y z" per line)
    result_pts  Path to write registered moving points (same format)
    reg_type    Registration type: rigid | affine | deformable
    max_iter    Maximum EM iterations (integer)
    tolerance   Convergence tolerance (float)
"""

import sys
import numpy as np


def load_points(path):
    points = []
    with open(path, 'r') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            parts = line.split()
            if len(parts) < 3:
                continue
            points.append([float(parts[0]), float(parts[1]), float(parts[2])])
    return np.array(points, dtype=np.float64)


def save_points(path, points):
    with open(path, 'w') as f:
        for p in points:
            f.write(f"{p[0]} {p[1]} {p[2]}\n")


def main():
    if len(sys.argv) < 7:
        print("Usage: cpd_registration.py <fixed_pts> <moving_pts> <result_pts> <reg_type> <max_iter> <tolerance>",
              file=sys.stderr)
        sys.exit(1)

    fixed_path   = sys.argv[1]
    moving_path  = sys.argv[2]
    result_path  = sys.argv[3]
    reg_type     = sys.argv[4]
    max_iter     = int(sys.argv[5])
    tolerance    = float(sys.argv[6])

    try:
        from pycpd import RigidRegistration, AffineRegistration, DeformableRegistration
    except ImportError:
        print("pycpd is not installed. Run: pip install pycpd", file=sys.stderr)
        sys.exit(1)

    fixed  = load_points(fixed_path)
    moving = load_points(moving_path)

    print(f"Fixed points:  {len(fixed)}")
    print(f"Moving points: {len(moving)}")
    print(f"Registration type: {reg_type}, max_iter: {max_iter}, tolerance: {tolerance}")

    reg_classes = {
        'rigid':       RigidRegistration,
        'affine':      AffineRegistration,
        'deformable':  DeformableRegistration,
    }

    if reg_type not in reg_classes:
        print(f"Unknown registration type '{reg_type}'. Use: rigid, affine, deformable", file=sys.stderr)
        sys.exit(1)

    reg = reg_classes[reg_type](X=fixed, Y=moving, max_iterations=max_iter, tolerance=tolerance)
    registered, _ = reg.register()

    save_points(result_path, registered)
    print("CPD registration complete.")


if __name__ == '__main__':
    main()
