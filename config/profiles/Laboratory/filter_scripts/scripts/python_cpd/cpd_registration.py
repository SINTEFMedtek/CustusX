"""
Coherent Point Drift (CPD) registration via pycpd.

Usage:
    python3 cpd_registration.py <fixed.txt> <moving.txt> <output.txt> [rigid|affine|deformable] [max_iter] [tolerance]

Each .txt file contains one point per line: "x y z"

Install dependency: pip install pycpd
"""
import sys
import numpy as np


def read_points(path):
    return np.loadtxt(path)


def write_points(path, points):
    np.savetxt(path, points, fmt='%.6f')


def run_cpd(fixed, moving, reg_type, max_iter, tolerance):
    from pycpd import RigidRegistration, AffineRegistration, DeformableRegistration

    kwargs = dict(X=fixed, Y=moving, max_iterations=max_iter, tolerance=tolerance)

    if reg_type == 'rigid':
        reg = RigidRegistration(**kwargs)
    elif reg_type == 'affine':
        reg = AffineRegistration(**kwargs)
    elif reg_type == 'deformable':
        reg = DeformableRegistration(**kwargs)
    else:
        raise ValueError(f"Unknown registration type: {reg_type}. Use rigid, affine, or deformable.")

    print(f"Starting {reg_type} CPD: {len(moving)} moving points -> {len(fixed)} fixed points", flush=True)
    TY, _ = reg.register()
    print("CPD registration complete.", flush=True)
    return TY


if __name__ == '__main__':
    if len(sys.argv) < 4:
        print("Usage: cpd_registration.py <fixed.txt> <moving.txt> <output.txt> [rigid|affine|deformable] [max_iter] [tolerance]")
        sys.exit(1)

    fixed_path = sys.argv[1]
    moving_path = sys.argv[2]
    output_path = sys.argv[3]
    reg_type = sys.argv[4] if len(sys.argv) > 4 else 'rigid'
    max_iter = int(sys.argv[5]) if len(sys.argv) > 5 else 100
    tolerance = float(sys.argv[6]) if len(sys.argv) > 6 else 1e-5

    try:
        fixed = read_points(fixed_path)
        moving = read_points(moving_path)
    except Exception as e:
        print(f"Error reading input files: {e}", flush=True)
        sys.exit(1)

    # Ensure 2D arrays with shape (N, 3)
    if fixed.ndim == 1:
        fixed = fixed.reshape(1, -1)
    if moving.ndim == 1:
        moving = moving.reshape(1, -1)

    try:
        result = run_cpd(fixed, moving, reg_type, max_iter, tolerance)
    except ImportError:
        print("ERROR: pycpd is not installed. Run: pip install pycpd", flush=True)
        sys.exit(1)
    except Exception as e:
        print(f"ERROR during CPD registration: {e}", flush=True)
        sys.exit(1)

    write_points(output_path, result)
    print(f"Result written to {output_path}", flush=True)
