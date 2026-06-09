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

Install dependency: pip install pycpd==2.0.0
"""
import sys
import numpy as np

_PYCPD_REQUIRED_VERSION = "2.0.0"

try:
    import importlib.metadata as _meta
    _installed = _meta.version("pycpd")
    if _installed != _PYCPD_REQUIRED_VERSION:
        print(f"ERROR: pycpd version mismatch: installed {_installed!r}, "
              f"required {_PYCPD_REQUIRED_VERSION!r}. "
              f"Delete the pycpd venv and let CustusX recreate it.", flush=True)
        sys.exit(1)
except Exception:
    pass  # pycpd not installed yet — ImportError raised later with a clear message


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
    """Mixin that forces scale=1 in pycpd's RigidRegistration EM iterations.

    Accesses private pycpd attributes (P, Np, X_hat, A, YPY, P1, R, s, t, D, N, M, Y, X).
    Verified against pycpd==2.0.0 only — see _PYCPD_REQUIRED_VERSION.
    """
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
    Normalise both point clouds using the MOVING cloud as the shared reference.

    Why the moving (US) cloud, not independent centering:
      Independent centering (fixed - mean_fixed, moving - mean_moving) places
      BOTH clouds at the origin.  This discards the approximate initial
      registration: CPD then searches for the moving cloud near the centroid
      of the FIXED (CT) cloud rather than at its actual approximate position —
      completely wrong when the US covers only a small sub-region of the CT.

    Why shared-reference normalisation works:
      Using the moving centroid as the common offset preserves the relative
      position (the approximate registration).  After normalisation:
        - The moving (US) cloud is centred at the origin.
        - The fixed (CT) cloud is shifted by the same amount, so the correct
          CT sub-region (where the US is already approximately placed) stays
          near the origin.
      CPD therefore starts iterating from the right neighbourhood.

    Why this also fixes w > 0 numerical failure:
      sigma2 is initialised by pycpd from all pairwise cross-cloud distances.
      Without normalisation those distances are dominated by the large
      inter-cloud translation (e.g. 150 mm), making sigma2 ≈ 22 500 mm²
      and the outlier constant c ≈ 800 000 — all posterior probabilities
      collapse to zero (Np = 0 → NaN → SVD failure).
      With shared-reference normalisation, the US cloud is at the origin and
      the relevant CT points are also near the origin, so pairwise distances
      are governed by the US cloud's own spread (~10 mm), making sigma2 ≈ 10–50
      and c ≈ 20–200 — well within the tractable range.

    Returns: X_n, Y_n, mean_ref, scale_ref
      Denormalise with: R_eff, t_orig = _denormalize(R, t_n, mean_ref, scale_ref, s)
    """
    mean_ref = np.mean(moving, axis=0)
    scale_ref = np.sqrt(np.mean(np.sum((moving - mean_ref) ** 2, axis=1)))
    if scale_ref < 1.0:   # guard against degenerate / single-point clouds
        scale_ref = 1.0

    # Both clouds shifted by the SAME moving centroid, scaled by the moving spread.
    X_n = (fixed - mean_ref) / scale_ref
    Y_n = (moving - mean_ref) / scale_ref   # centred at origin, unit RMS radius

    return X_n, Y_n, mean_ref, scale_ref


def _denormalize(R, t_n, mean_ref, scale_ref, s=1.0):
    """
    Recover (R_eff, t_orig) in original coordinate space, including scale s.

    Derivation (row-vector convention):
      T_norm(y_n) = s * y_n @ R + t_n      (pycpd, normalised space)
      y_n = (y - mean_ref) / scale_ref
      T_orig(y) = scale_ref * T_norm((y - mean_ref) / scale_ref) + mean_ref
               = s * (y - mean_ref) @ R + scale_ref * t_n + mean_ref
               = y @ (s * R) + (mean_ref - s * mean_ref @ R + scale_ref * t_n)

      R_eff  = s * R   (pure rotation for s=1, similarity rotation for s≠1)
      t_orig = mean_ref - s * mean_ref @ R + scale_ref * t_n
    """
    R_eff = s * R
    t_orig = mean_ref - s * np.dot(mean_ref, R) + scale_ref * t_n
    return R_eff, t_orig


def _run_rigid_normalized(X_n, Y_n, max_iter, tolerance, w):
    """Run rigid CPD (no scale) on pre-normalised data. Returns (R, t_n)."""
    from pycpd import RigidRegistration

    class RigidRegistrationNoScale(_RigidRegistrationNoScale, RigidRegistration):
        pass

    reg = RigidRegistrationNoScale(X=X_n, Y=Y_n,
                                   max_iterations=max_iter, tolerance=tolerance, w=w)
    reg.register()
    return reg.R, reg.t


def _run_scaled_normalized(X_n, Y_n, max_iter, tolerance, w):
    """Run rigid CPD with scale on pre-normalised data. Returns (R, t_n, s)."""
    from pycpd import RigidRegistration
    reg = RigidRegistration(X=X_n, Y=Y_n,
                            max_iterations=max_iter, tolerance=tolerance, w=w)
    reg.register()
    return reg.R, reg.t, reg.s


def run_rigid_cpd(fixed, moving, max_iter, tolerance, w, scale_mode, scale_threshold):
    print(f"Starting rigid CPD: {len(moving)} moving -> {len(fixed)} fixed points, "
          f"mode={scale_mode}, w={w}, tol={tolerance}", flush=True)

    X_n, Y_n, mean_ref, scale_ref = _preprocess(fixed, moving)

    if scale_mode == "Rigid+scale":
        R, t_n, s = _run_scaled_normalized(X_n, Y_n, max_iter, tolerance, w)
        print(f"CPD complete (rigid+scale). Scale s={s:.4f}", flush=True)
        R_eff, t = _denormalize(R, t_n, mean_ref, scale_ref, s)

    elif scale_mode == "Auto":
        R, t_n, s = _run_scaled_normalized(X_n, Y_n, max_iter, tolerance, w)
        if abs(s - 1.0) > scale_threshold:
            print(f"CPD auto: scale s={s:.4f} exceeds threshold {scale_threshold}, "
                  f"rerunning without scale.", flush=True)
            R, t_n = _run_rigid_normalized(X_n, Y_n, max_iter, tolerance, w)
            R_eff, t = _denormalize(R, t_n, mean_ref, scale_ref, 1.0)
        else:
            print(f"CPD auto: scale s={s:.4f} within threshold {scale_threshold}.", flush=True)
            R_eff, t = _denormalize(R, t_n, mean_ref, scale_ref, s)

    else:  # "Rigid" (default)
        R, t_n = _run_rigid_normalized(X_n, Y_n, max_iter, tolerance, w)
        print("CPD complete (rigid, no scale).", flush=True)
        R_eff, t = _denormalize(R, t_n, mean_ref, scale_ref, 1.0)

    return R_eff, t


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
