
# cx/windows_env.py
# Setup build environmet for Windows
# Allows cxInstaller.py to be run directly, not needing Setup-Custusx.ps1 to be run every time.
# Setup-Custusx.ps1 still needs to be run once (as admin) to verify/install tools
# Made mostly by Copilot

import os
import json
import subprocess
import shutil
from pathlib import Path
from typing import Dict, Optional, Tuple


def _detect_qt_root(qt_path: Optional[str], qt_version: str, qt_module: str) -> Optional[str]:
    """
    Mimic the PS script: detect Qt by looking for qmake.exe under a few standard locations.
    Returns the Qt root (e.g. C:/Qt/5.15.2/msvc2019_64) or None.
    """
    # If user passed an explicit path, trust it if it looks valid
    if qt_path:
        p = Path(qt_path)
        if (p / "bin" / "qmake.exe").exists():
            return str(p)

    candidates = [
        rf"D:\Qt\{qt_version}\{qt_module}",
        rf"C:\Qt\{qt_version}\{qt_module}",
        rf"D:\Qt\{qt_version}",
        rf"C:\Qt\{qt_version}",
    ]
    for c in candidates:
        if (Path(c) / "bin" / "qmake.exe").exists():
            return c
    return None

def apply_custusx_extra_env(qt_path: Optional[str] = "",
                            qt_version: str = "5.15.2",
                            qt_module: str = "msvc2019_64",
                            force_ninja: bool = True,
                            set_cxx17: bool = True,
                            disable_docs: bool = True,
                            disable_git_sh: bool = True) -> None:
    """
    Replicates the environment part of Setup-CustusX.ps1:
      - Detect Qt and set QT_DIR, Qt5_DIR, PATH, and CMAKE_PREFIX_PATH
      - Prefer Ninja generator and set C++17 flags
      - Optionally sanitize CMake's use of sh.exe on Windows (Git for Windows)
      - Turn off docs for ITK/VTK (matches PS script)
    """
    # 1) Qt detection & env
    qt_root = _detect_qt_root(qt_path, qt_version, qt_module)
    if not qt_root:
        raise RuntimeError(
            f"Qt {qt_version} not found. Provide --qt-path or install Qt {qt_version} {qt_module}."
        )
    os.environ["QT_DIR"] = qt_root
    qt_bin = str(Path(qt_root) / "bin")
    if qt_bin not in os.environ.get("PATH", ""):
        os.environ["PATH"] = f"{qt_bin};{os.environ.get('PATH','')}"
    qt5_cmake = str(Path(qt_root) / "lib" / "cmake" / "Qt5")
    if Path(qt5_cmake).exists():
        os.environ["Qt5_DIR"] = qt5_cmake
        # Prepend Qt to CMAKE_PREFIX_PATH so nested CMake (VTK, etc.) sees it immediately
        existing = os.environ.get("CMAKE_PREFIX_PATH", "")
        os.environ["CMAKE_PREFIX_PATH"] = f"{qt5_cmake};{existing}" if existing else qt5_cmake

    # 2) Generator & compiler flags (Ninja + C++17)
    if force_ninja:
        os.environ["CMAKE_GENERATOR"] = "Ninja"
        # Point CMake explicitly to Ninja if available
        ninja_path = shutil.which("ninja")
        if ninja_path:
            os.environ["CMAKE_MAKE_PROGRAM"] = ninja_path
    if set_cxx17:
        os.environ["CMAKE_CXX_STANDARD"] = "17"
        os.environ["CMAKE_CXX_STANDARD_REQUIRED"] = "ON"
        os.environ["CMAKE_CXX_EXTENSIONS"] = "OFF"

    # 3) Optional: help CMake ignore Git's sh.exe with MSVC on Windows
    # (the PS file doesn't do this; we add it because it avoids weird try_compile failures)
    if disable_git_sh and os.name == "nt":
        os.environ["CMAKE_SH"] = "CMAKE_SH-NOTFOUND"

    # 4) Turn OFF documentation builds (matches PS script)
    if disable_docs:
        os.environ["ITK_BUILD_DOCUMENTATION"] = "OFF"
        os.environ["VTK_BUILD_DOCUMENTATION"] = "OFF"


def _find_vswhere() -> Optional[str]:
    candidates = [
        r"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe",
        r"C:\Program Files\Microsoft Visual Studio\Installer\vswhere.exe",
    ]
    for p in candidates:
        if os.path.isfile(p):
            return p
    return None

def find_vs_paths() -> Tuple[Optional[str], Optional[str]]:
    """
    Return (vcvars64_bat, vsdevcmd_bat) using vswhere first, then fallbacks.
    """
    vcvars, vsdev = None, None
    vswhere = _find_vswhere()
    if vswhere:
        args = [
            vswhere, "-latest", "-products", "*",
            "-requires", "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
            "-format", "json",
        ]
        try:
            out = subprocess.check_output(args, text=True)
            data = json.loads(out)
            if data:
                root = Path(data[0]["installationPath"])
                vc = root / "VC" / "Auxiliary" / "Build" / "vcvars64.bat"
                vd = root / "Common7" / "Tools" / "VsDevCmd.bat"
                vcvars = str(vc) if vc.exists() else None
                vsdev = str(vd) if vd.exists() else None
        except Exception:
            pass

    # Fallback scan if vswhere unavailable
    if not vcvars:
        roots = [
            r"C:\Program Files\Microsoft Visual Studio\2022",
            r"C:\Program Files (x86)\Microsoft Visual Studio\2022",
            r"C:\Program Files\Microsoft Visual Studio\2019",
            r"C:\Program Files (x86)\Microsoft Visual Studio\2019",
            r"C:\Program Files\Microsoft Visual Studio\2017",
            r"C:\Program Files (x86)\Microsoft Visual Studio\2017",
        ]
        editions = ("Enterprise", "Professional", "Community", "BuildTools")
        for root in roots:
            for ed in editions:
                candidate = Path(root) / ed / "VC" / "Auxiliary" / "Build" / "vcvars64.bat"
                if candidate.exists():
                    vcvars = str(candidate)
                    break
            if vcvars:
                break

    if not vsdev:
        roots = [
            r"C:\Program Files\Microsoft Visual Studio\2022",
            r"C:\Program Files (x86)\Microsoft Visual Studio\2022",
            r"C:\Program Files\Microsoft Visual Studio\2019",
            r"C:\Program Files (x86)\Microsoft Visual Studio\2019",
        ]
        editions = ("Enterprise", "Professional", "Community", "BuildTools")
        for root in roots:
            for ed in editions:
                candidate = Path(root) / ed / "Common7" / "Tools" / "VsDevCmd.bat"
                if candidate.exists():
                    vsdev = str(candidate)
                    break
            if vsdev:
                break

    return vcvars, vsdev

def get_env_from_batch(batch_path: str, batch_args: str = "") -> Dict[str, str]:
    """
    Call the batch file (vcvars64.bat or VsDevCmd.bat) and capture environment via 'set'.
    """
    call_line = f'call "{batch_path}" {batch_args}'.strip()
    cmd = f'cmd /s /c "{call_line} >nul && set"'
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True, text=True)
    stdout, stderr = proc.communicate()
    if proc.returncode != 0:
        raise RuntimeError(f"Failed to get environment from {batch_path}: {stderr}")
    env: Dict[str, str] = {}
    for line in stdout.splitlines():
        if "=" in line:
            k, v = line.split("=", 1)
            env[k] = v
    return env

def apply_msvc_env(prefer: str = "vcvars", explicit_batch: Optional[str] = None) -> str:
    """
    Import the MSVC dev environment into the current Python process (os.environ).
    prefer: 'vcvars' (default) or 'vsdev'
    explicit_batch: optional explicit path to a batch file.
    Returns the batch file path used.
    """
    vcvars, vsdev = find_vs_paths()
    batch_path = explicit_batch or (vcvars if (prefer == "vcvars" and vcvars) else vsdev or vcvars)
    if not batch_path:
        raise FileNotFoundError("Could not locate Visual Studio dev environment (vcvars64.bat/VsDevCmd.bat).")

    # Ensure x64 arch when using VsDevCmd (it defaults to the latest, but be explicit)
    batch_args = ""
    if Path(batch_path).name.lower() == "vsdevcmd.bat":
        batch_args = "-arch=x64 -host_arch=x64"

    env = get_env_from_batch(batch_path, batch_args=batch_args)
    os.environ.update(env)
    return batch_path

def run_in_msvc_env(commands, prefer: str = "vcvars", explicit_batch: Optional[str] = None, cwd: Optional[str] = None) -> None:
    """
    Chain commands after initializing the VS env. Prefer vcvars64.bat for consistency.
    """
    vcvars, vsdev = find_vs_paths()
    batch_path = explicit_batch or (vcvars if (prefer == "vcvars" and vcvars) else vsdev or vcvars)
    if not batch_path:
        raise FileNotFoundError("Could not locate Visual Studio dev environment (vcvars64.bat/VsDevCmd.bat).")
    batch_args = ""
    if Path(batch_path).name.lower() == "vsdevcmd.bat":
        batch_args = "-arch=x64 -host_arch=x64"

    chained = " && ".join(commands)
    cmdline = f'cmd /s /c "call \\"{batch_path}\\" {batch_args} && {chained}"'
    result = subprocess.run(cmdline, cwd=cwd, shell=True)
    if result.returncode != 0:
        raise RuntimeError(f"Commands failed with code {result.returncode}")

def verify_msvc_tools() -> None:
    """
    Quick sanity check after apply_msvc_env(): ensure cl.exe, cmake, ninja are found.
    """
    for tool in ("cl", "cmake", "ninja"):
        r = subprocess.run(["where", tool], capture_output=True, text=True, shell=True)
        if r.returncode != 0:
            raise RuntimeError(f"Tool not found in PATH after applying MSVC env: {tool}\n{r.stderr}")
