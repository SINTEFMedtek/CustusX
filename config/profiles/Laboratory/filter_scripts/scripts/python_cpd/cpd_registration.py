"""
Coherent Point Drift (CPD) rigid registration via pycpd.

Usage:
    python cpd_registration.py <fixed.txt> <moving.txt> <output.txt>
        [max_iter] [tolerance] [w] [scale_mode] [scale_threshold]

Each .txt file contains one point per line: "x y z"
Output file contains the rigid transform: 3 rows of R (rotation), then 1 row of t (translation).

Arguments:
    max_iter       Maximum EM iterations (default: 100)
    tolerance      Convergence tolerance (default: 1e-3, pycpd default)
    w              Outlier weight [0, 0.9] — fraction of points treated as noise (default: 0.0)
    scale_mode     "Rigid", "Rigid+scale", or "Auto" (default: "Rigid")
    scale_threshold In Auto mode, fall back to Rigid if |scale - 1| > threshold (default: 0.1)

Install dependency: pip install pycpd
"""
import sys
import numpy as np


def read_points(path):
    return np.loadtxt(path)


def write_transform(path, R, t):
    # pycpd uses row-vector convention: TY = Y @ R + t
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


def _preprocess(fixed, moving):
    """
    Center both clouds to zero mean and scale to unit RMS radius.

    Pre-normalization stabilises pycpd's E-step, particularly when w > 0.
    Without it, sigma2 is initialised from all pairwise distances and can be
    enormous when the clouds are far apart, making the outlier constant c so
    large that all posterior probabilities collapse to zero (Np = 0 → NaN / SVD
    failure).

    Rotation is invariant to centering and isotropic scaling, so R from the
    normalised run is identical to R from the original run. The translation is
    recovered in the original space via centroid mapping after the run.
    """
    mean_X = np.mean(fixed, axis=0)
    mean_Y = np.mean(moving, axis=0)
    fixed_c = fixed - mean_X
    moving_c = moving - mean_Y
    rms_X = np.sqrt(np.mean(np.sum(fixed_c ** 2, axis=1)))
    rms_Y = np.sqrt(np.mean(np.sum(moving_c ** 2, axis=1)))
    scale = (rms_X + rms_Y) / 2.0
    if scale < 1e-10:
        scale = 1.0
    return fixed_c / scale, moving_c / scale, mean_X, mean_Y


def _run_rigid_normalized(X_n, Y_n, max_iter, tolerance, w):
    """Run rigid CPD (no scale) on pre-normalised data. Returns R."""
    from pycpd import RigidRegistration

    class RigidRegistrationNoScale(_RigidRegistrationNoScale, RigidRegistration):
        pass

    reg = RigidRegistrationNoScale(X=X_n, Y=Y_n,
                                   max_iterations=max_iter, tolerance=tolerance, w=w)
    reg.register()
    return reg.R


def _run_scaled_normalized(X_n, Y_n, max_iter, tolerance, w):
    """Run rigid CPD with scale on pre-normalised data. Returns (R, s)."""
    from pycpd import RigidRegistration
    reg = RigidRegistration(X=X_n, Y=Y_n,
                            max_iterations=max_iter, tolerance=tolerance, w=w)
    reg.register()
    return reg.R, reg.s


def run_rigid_cpd(fixed, moving, max_iter, tolerance, w, scale_mode, scale_threshold):
    print(f"Starting rigid CPD: {len(moving)} moving -> {len(fixed)} fixed points, "
          f"mode={scale_mode}, w={w}, tol={tolerance}", flush=True)

    # Normalise both clouds before running CPD for numerical stability.
    # Rotation is recovered from the normalised run; translation is computed
    # from the original centroids afterwards, which is exact for rigid registration.
    X_n, Y_n, mean_X, mean_Y = _preprocess(fixed, moving)

    if scale_mode == "Rigid+scale":
        R, s = _run_scaled_normalized(X_n, Y_n, max_iter, tolerance, w)
        print(f"CPD complete (rigid+scale). Scale s={s:.4f}", flush=True)

    elif scale_mode == "Auto":
        R, s = _run_scaled_normalized(X_n, Y_n, max_iter, tolerance, w)
        if abs(s - 1.0) > scale_threshold:
            print(f"CPD auto: scale s={s:.4f} exceeds threshold {scale_threshold}, "
                  f"rerunning without scale.", flush=True)
            R = _run_rigid_normalized(X_n, Y_n, max_iter, tolerance, w)
        else:
            print(f"CPD auto: scale s={s:.4f} within threshold {scale_threshold}.", flush=True)

    else:  # "Rigid" (default)
        R = _run_rigid_normalized(X_n, Y_n, max_iter, tolerance, w)
        print("CPD complete (rigid, no scale).", flush=True)

    # Translation: map centroid of moving to centroid of fixed under rotation R.
    # Exact for rigid registration regardless of whether scale was used to find R.
    # Row-vector form: t = mean_X - mean_Y @ R
    t = mean_X - np.dot(mean_Y, R)
    return R, t


if __name__ == '__main__':
    if len(sys.argv) < 4:
        print("Usage: cpd_registration.py <fixed.txt> <moving.txt> <output.txt> "
              "[max_iter] [tolerance] [w] [scale_mode] [scale_threshold]")
        sys.exit(1)

    fixed_path = sys.argv[1]
    moving_path = sys.argv[2]
    output_path = sys.argv[3]
    max_iter = int(sys.argv[4]) if len(sys.argv) > 4 else 100
    tolerance = float(sys.argv[5]) if len(sys.argv) > 5 else 1e-3
    w = float(sys.argv[6]) if len(sys.argv) > 6 else 0.0
    scale_mode = sys.argv[7] if len(sys.argv) > 7 else "Rigid"
    scale_threshold = float(sys.argv[8]) if len(sys.argv) > 8 else 0.1

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
        R, t = run_rigid_cpd(fixed, moving, max_iter, tolerance, w, scale_mode, scale_threshold)
    except ImportError:
        print("ERROR: pycpd is not installed. Run: pip install pycpd", flush=True)
        sys.exit(1)
    except Exception as e:
        print(f"ERROR during CPD registration: {e}", flush=True)
        sys.exit(1)

    write_transform(output_path, R, t)
    print(f"Transform written to {output_path}", flush=True)
