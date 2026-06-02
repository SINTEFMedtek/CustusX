"""
Coherent Point Drift (CPD) rigid registration via pycpd.

Usage:
    python cpd_registration.py <fixed.txt> <moving.txt> <output.txt> [max_iter] [tolerance]

Each .txt file contains one point per line: "x y z"
Output file contains the rigid transform: 3 rows of R (rotation), then 1 row of t (translation).

Install dependency: pip install pycpd
"""
import sys
import numpy as np


def read_points(path):
    return np.loadtxt(path)


def write_transform(path, R, t):
    # pycpd uses row-vector convention: TY = s * Y @ R + t
    # CustusX/Eigen uses column-vector convention: T(y) = R_col @ y + t
    # R_col = R.T
    R_col = R.T
    with open(path, 'w') as f:
        for row in R_col:
            f.write(f"{row[0]:.10f} {row[1]:.10f} {row[2]:.10f}\n")
        f.write(f"{t[0]:.10f} {t[1]:.10f} {t[2]:.10f}\n")


class _RigidRegistrationNoScale:
    """Mixin that forces scale=1 in pycpd's RigidRegistration EM iterations."""
    def update_transform(self):
        muX = np.divide(np.sum(np.dot(self.P, self.X), axis=0), self.Np)
        muY = np.divide(np.sum(np.dot(np.transpose(self.P), self.Y), axis=0), self.Np)
        self.X_hat = self.X - np.tile(muX, (self.N, 1))
        Y_hat = self.Y - np.tile(muY, (self.M, 1))
        self.YPY = np.dot(np.transpose(self.P1),
                          np.sum(np.multiply(Y_hat, Y_hat), axis=1))
        self.A = np.dot(np.transpose(self.X_hat),
                        np.dot(np.transpose(self.P), Y_hat))
        U, _, V = np.linalg.svd(self.A, full_matrices=True)
        C = np.ones((self.D,))
        C[self.D - 1] = np.linalg.det(np.dot(U, V))
        self.R = np.transpose(np.dot(np.dot(U, np.diag(C)), V))
        self.s = 1.0
        self.t = np.transpose(muX) - np.dot(np.transpose(self.R), np.transpose(muY))


def run_rigid_cpd(fixed, moving, max_iter, tolerance):
    from pycpd import RigidRegistration

    class RigidRegistrationNoScale(_RigidRegistrationNoScale, RigidRegistration):
        pass

    reg = RigidRegistrationNoScale(X=fixed, Y=moving, max_iterations=max_iter, tolerance=tolerance)

    print(f"Starting rigid CPD: {len(moving)} moving points -> {len(fixed)} fixed points", flush=True)
    reg.register()
    print("CPD registration complete.", flush=True)

    return reg.R, reg.t


if __name__ == '__main__':
    if len(sys.argv) < 4:
        print("Usage: cpd_registration.py <fixed.txt> <moving.txt> <output.txt> [max_iter] [tolerance]")
        sys.exit(1)

    fixed_path = sys.argv[1]
    moving_path = sys.argv[2]
    output_path = sys.argv[3]
    max_iter = int(sys.argv[4]) if len(sys.argv) > 4 else 100
    tolerance = float(sys.argv[5]) if len(sys.argv) > 5 else 1e-5

    try:
        fixed = read_points(fixed_path)
        moving = read_points(moving_path)
    except Exception as e:
        print(f"Error reading input files: {e}", flush=True)
        sys.exit(1)

    if fixed.ndim == 1:
        fixed = fixed.reshape(1, -1)
    if moving.ndim == 1:
        moving = moving.reshape(1, -1)

    try:
        R, t = run_rigid_cpd(fixed, moving, max_iter, tolerance)
    except ImportError:
        print("ERROR: pycpd is not installed. Run: pip install pycpd", flush=True)
        sys.exit(1)
    except Exception as e:
        print(f"ERROR during CPD registration: {e}", flush=True)
        sys.exit(1)

    write_transform(output_path, R, t)
    print(f"Transform written to {output_path}", flush=True)
