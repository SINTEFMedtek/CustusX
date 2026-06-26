
<#
.SYNOPSIS
  One-time tool installer for the CustusX Windows build environment.

.DESCRIPTION
  Run this script ONCE (as Administrator) to install required build tools:
  - Verifies/installs Python, CMake, Git, and Ninja via winget.
  - Detects Qt 5.15.2 (default D:\Qt\5.15.2\msvc2019_64, configurable via -QtPath).
  - Installs GLEW via vcpkg.

  After running this script, you do NOT need to run it again before each build.
  cxInstaller.py (and the other build scripts) automatically detect and initialize
  the Visual Studio environment at runtime via cx/windows_env.py.

.NOTES
  - If scripts are blocked: Set-ExecutionPolicy RemoteSigned -Scope CurrentUser
  - Run as Administrator to allow automatic winget/vcpkg installs.
#>

param(
    [string] $QtPath             = "",                 # e.g. "D:\Qt\5.15.2\msvc2019_64"
    [string] $QtVersion          = "5.15.2",
    [string] $QtModule           = "msvc2019_64",
    [string] $BuildType          = "Release",
    [switch] $SkipWingetInstalls,                       # Skip tool installs even if missing
    [switch] $VerboseEnvSummary                          # Show full environment summary
)

Write-Host "=== CustusX Windows Setup (Ninja) ==="

# ------------------------------------------------------------
# Helpers
# ------------------------------------------------------------
function Test-IsAdmin {
    $wi = [Security.Principal.WindowsIdentity]::GetCurrent()
    $wp = [Security.Principal.WindowsPrincipal]::new($wi)
    return $wp.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
}
$IsAdmin = Test-IsAdmin

function Install-PackageIfMissing {
    param(
        [Parameter(Mandatory)][string]$CmdName,
        [Parameter(Mandatory)][string]$WingetId,
        [string]$VerifyCmd = $CmdName
    )
    if (Get-Command $VerifyCmd -ErrorAction SilentlyContinue) {
        Write-Host "$CmdName already installed."
        return
    }
    if ($SkipWingetInstalls) {
        Write-Warning "$CmdName not found and -SkipWingetInstalls set. Please install it and re-run."
        return
    }
    if (-not $IsAdmin) {
        Write-Warning "$CmdName not found and this shell is not elevated. Re-run as Administrator or install $CmdName manually."
        return
    }
    if (-not (Get-Command winget -ErrorAction SilentlyContinue)) {
        Write-Warning "winget is not available. Install tools manually or use a newer Windows."
        return
    }
    Write-Host "Installing $CmdName via winget..."
    winget install --id $WingetId --silent --accept-source-agreements --accept-package-agreements | Out-Null
    if (Get-Command $VerifyCmd -ErrorAction SilentlyContinue) {
        Write-Host "$CmdName installed."
    } else {
        Write-Warning "Failed to install $CmdName. Please install manually and re-run."
    }
}

# Import env from vcvars64.bat / VsDevCmd.bat into current PS process
function Import-BatchEnvironment {
    param(
        [Parameter(Mandatory)][string]$BatchFile,
        [string]$Arguments = ""
    )
    if (-not (Test-Path $BatchFile)) {
        throw "Batch file not found: $BatchFile"
    }
    $cmdLine = "`"$BatchFile`" $Arguments && set"

    $psi = New-Object System.Diagnostics.ProcessStartInfo
    $psi.FileName = "cmd.exe"
    $psi.Arguments = "/c $cmdLine"
    $psi.UseShellExecute = $false
    $psi.RedirectStandardOutput = $true
    $psi.RedirectStandardError  = $true

    $p = New-Object System.Diagnostics.Process
    $p.StartInfo = $psi
    $null = $p.Start()
    $stdout = $p.StandardOutput.ReadToEnd()
    $stderr = $p.StandardError.ReadToEnd()
    $p.WaitForExit()

    if ($p.ExitCode -ne 0) {
        throw "Failed to import environment from $BatchFile. Error: $stderr"
    }

    foreach ($line in $stdout -split "`r?`n") {
        if ($line -match "^(.*?)=(.*)$") {
            $name  = $matches[1]
            $value = $matches[2]
            Set-Item -Path ("Env:{0}" -f $name) -Value $value
        }
    }
}

# ------------------------------------------------------------
# 1) Ensure core tools (Python, CMake, Git, Ninja)
# ------------------------------------------------------------
Install-PackageIfMissing -CmdName "python" -WingetId "Python.Python.3"
Install-PackageIfMissing -CmdName "cmake"  -WingetId "Kitware.CMake"
Install-PackageIfMissing -CmdName "git"    -WingetId "Git.Git"
Install-PackageIfMissing -CmdName "ninja"  -WingetId "Ninja-build.Ninja"

# Verify versions
$cmakeVer = ""
try {
    $cmakeLine = (cmake --version | Select-Object -First 1)
    if ($cmakeLine -match "cmake version ([0-9]+\.[0-9]+\.[0-9]+)") { $cmakeVer = $Matches[1] }
} catch {}
if ($cmakeVer -ne "") {
    Write-Host "CMake version: $cmakeVer"
    # recommend >= 3.28 for smooth MSVC 19.4x + Ninja + features tables
    [Version]$req = [Version]"3.28.0"
    [Version]$cur = [Version]$cmakeVer
    if ($cur -lt $req) {
        Write-Warning "CMake $cmakeVer detected (< 3.28). Consider upgrading for best Ninja/MSVC compatibility."
    }
} else {
    Write-Warning "Unable to determine CMake version."
}

if (Get-Command ninja -ErrorAction SilentlyContinue) {
    Write-Host ("Ninja version: " + ((ninja --version)))
} else {
    Write-Warning "Ninja not found on PATH."
}

# ------------------------------------------------------------
# 2) Locate VS 2022 and import MSVC env (vcvars64.bat)
# ------------------------------------------------------------
Write-Host "Checking Visual Studio 2022..."
$vswhere = Join-Path "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer" "vswhere.exe"
if (-not (Test-Path $vswhere)) {
    Write-Warning "vswhere.exe not found. Please install Visual Studio Installer / VS 2022."
    exit 1
}

$vsInstall = & $vswhere -version "[17.0,18.0)" -products * `
    -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
    -property installationPath 2>$null

if (-not $vsInstall) {
    Write-Warning "Visual Studio 2022 with C++ workload not found."
    Write-Warning "Install VS 2022 + 'Desktop development with C++' workload, then re-run."
    exit 1
}
Write-Host "Visual Studio found at $vsInstall"

$vcvars   = Join-Path $vsInstall "VC\Auxiliary\Build\vcvars64.bat"
$vsdevcmd = Join-Path $vsInstall "Common7\Tools\VsDevCmd.bat"

try {
    if (Test-Path $vcvars) {
        Write-Host "Importing MSVC environment from: $vcvars"
        Import-BatchEnvironment -BatchFile $vcvars
    } elseif (Test-Path $vsdevcmd) {
        Write-Host "Importing MSVC environment from: $vsdevcmd"
        Import-BatchEnvironment -BatchFile $vsdevcmd -Arguments "-no_logo"
    } else {
        throw "Neither vcvars64.bat nor VsDevCmd.bat found under $vsInstall"
    }
} catch {
    Write-Warning $_.Exception.Message
    Write-Warning "Open 'Developer PowerShell for VS 2022' and re-run."
    exit 1
}

if (-not (Get-Command cl.exe -ErrorAction SilentlyContinue)) {
    Write-Warning "cl.exe not found on PATH after import. Aborting."
    exit 1
}
Write-Host "MSVC environment imported. cl.exe available."

# ------------------------------------------------------------
# 3) Detect Qt 5.15.2 and set env hints for CMake
# ------------------------------------------------------------
if (-not $QtPath -or $QtPath.Trim() -eq "") {
    $QtPathCandidates = @(
        "D:\Qt\$QtVersion\$QtModule",
        "C:\Qt\$QtVersion\$QtModule",
        "D:\Qt\$QtVersion",
        "C:\Qt\$QtVersion"
    )
    foreach ($p in $QtPathCandidates) {
        if ((Test-Path $p) -and (Test-Path (Join-Path $p "bin\qmake.exe"))) {
            $QtPath = $p
            break
        }
    }
}
if (-not $QtPath -or -not (Test-Path $QtPath)) {
    Write-Warning "Qt $QtVersion not found. Provide -QtPath or install Qt 5.15.2 msvc2019_64."
    Write-Warning "Expected something like D:\Qt\$QtVersion\$QtModule\bin\qmake.exe"
    exit 1
}
Write-Host "Qt detected at: $QtPath"
$env:QT_DIR = $QtPath
$qtBin = Join-Path $QtPath "bin"
if ($env:PATH -notlike "*$qtBin*") {
    $env:PATH = "$qtBin;$env:PATH"
}
$qt5CMake = Join-Path $QtPath "lib\cmake\Qt5"
if (Test-Path $qt5CMake) {
    $env:Qt5_DIR = $qt5CMake
    # Help nested CMake runs (VTK, etc.) find Qt immediately
    $env:CMAKE_PREFIX_PATH = "$($env:Qt5_DIR);$($env:CMAKE_PREFIX_PATH)"
}


# ------------------------------------------------------------
# vcpkg (classic mode) helpers
# ------------------------------------------------------------
function Ensure-VcpkgClassic {
    param(
        [string] $VcpkgRoot = "C:\src\vcpkg"  # adjust if you prefer another location
    )

    $vcpkgExe = Join-Path $VcpkgRoot "vcpkg.exe"
    if (Test-Path $vcpkgExe) {
        Write-Host "vcpkg found at: $vcpkgExe"
        return $vcpkgExe
    }

    # If not present, try to clone and bootstrap a standalone instance (classic mode).
    if (-not (Test-Path $VcpkgRoot)) {
        if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
            Write-Warning "git is required to clone vcpkg but was not found on PATH."
            Write-Warning "Install Git and re-run, or pre-create $VcpkgRoot with a vcpkg clone."
            return $null
        }

        Write-Host "Cloning vcpkg to $VcpkgRoot..."
        git clone https://github.com/microsoft/vcpkg $VcpkgRoot 2>$null
        if ($LASTEXITCODE -ne 0 -or -not (Test-Path $VcpkgRoot)) {
            Write-Warning "Failed to clone vcpkg. Please clone it manually to $VcpkgRoot and re-run."
            return $null
        }
    }

    $bootstrapBat = Join-Path $VcpkgRoot "bootstrap-vcpkg.bat"
    if (-not (Test-Path $bootstrapBat)) {
        Write-Warning "bootstrap-vcpkg.bat not found under $VcpkgRoot. Is this a valid vcpkg clone?"
        return $null
    }

    Write-Host "Bootstrapping vcpkg (classic mode)..."
    & $bootstrapBat -disableMetrics
    if ($LASTEXITCODE -ne 0) {
        Write-Warning "vcpkg bootstrap failed. Check your environment and try again."
        return $null
    }

    if (Test-Path $vcpkgExe) {
        Write-Host "vcpkg bootstrapped at: $vcpkgExe"
        return $vcpkgExe
    } else {
        Write-Warning "vcpkg.exe not found after bootstrap."
        return $null
    }
}

# ------------------------------------------------------------
# Ensure standalone vcpkg, then install GLEW (classic mode)
# ------------------------------------------------------------
$VcpkgRoot = "C:\src\vcpkg"  # adjust if needed
$vcpkgExe  = Ensure-VcpkgClassic -VcpkgRoot $VcpkgRoot

if ($vcpkgExe) {
    Write-Host "Installing GLEW via vcpkg (classic mode)..."
    & $vcpkgExe install "glew:x64-windows"

    if ($LASTEXITCODE -eq 0) {
        # Tell CMake to use vcpkg's toolchain so GLEW and other ports are found
        $env:CMAKE_TOOLCHAIN_FILE = Join-Path $VcpkgRoot "scripts\buildsystems\vcpkg.cmake"
        Write-Host "Set CMAKE_TOOLCHAIN_FILE to: $env:CMAKE_TOOLCHAIN_FILE"
    } else {
        Write-Warning "GLEW installation failed. Check vcpkg output above."
    }
} else {
    Write-Warning "Skipping GLEW install because vcpkg could not be ensured."
}


# ------------------------------------------------------------
# 4) Force fast generator + safe language standard for compatibility
# ------------------------------------------------------------
$env:CMAKE_GENERATOR           = "Ninja"
$env:CMAKE_CXX_STANDARD        = "17"
$env:CMAKE_CXX_STANDARD_REQUIRED = "ON"
$env:CMAKE_CXX_EXTENSIONS      = "OFF"

# Optional: help CMake find ninja explicitly (not usually required if on PATH)
try {
    $ninjaPath = (Get-Command ninja -ErrorAction SilentlyContinue).Source
    if ($ninjaPath) { $env:CMAKE_MAKE_PROGRAM = $ninjaPath }
} catch {}

# ------------------------------------------------------------
# 5) Environment summary
# ------------------------------------------------------------
Write-Host "`n--- Environment Summary ---"
Write-Host ("cl.exe path      : " + (Get-Command cl.exe).Source)
Write-Host ("CMake            : " + ((cmake --version | Select-Object -First 1)))
Write-Host ("Ninja            : " + ((ninja --version 2>$null) | Out-String).Trim())
Write-Host ("Python           : " + ((python --version 2>&1)))
Write-Host ("Git              : " + ((git --version 2>&1)))
Write-Host ("Qt dir (QT_DIR)  : $env:QT_DIR")
Write-Host ("Qt5_DIR          : $env:Qt5_DIR")
Write-Host ("CMAKE_GENERATOR  : $env:CMAKE_GENERATOR")
Write-Host ("CMAKE_PREFIX_PATH: $env:CMAKE_PREFIX_PATH")
Write-Host ("C++ standard     : $env:CMAKE_CXX_STANDARD (required=$env:CMAKE_CXX_STANDARD_REQUIRED, extensions=$env:CMAKE_CXX_EXTENSIONS)")
if ($VerboseEnvSummary) {
    Write-Host "`nFull PATH:"; Write-Host $env:PATH
    Write-Host "`n--- End Summary ---`n"
} else {
    Write-Host "---------------------------`n"
}

# Remove the vcpkg toolchain before running CustusX python/CMake 
Remove-Item Env:CMAKE_TOOLCHAIN_FILE


# Turn OFF docs to avoid Documentation.cmake + Perl
$env:ITK_BUILD_DOCUMENTATION = "OFF"
$env:VTK_BUILD_DOCUMENTATION = "OFF"

# Adopt a policy window compatible with VTK's KWSys on modern CMake
#$env:CMAKE_POLICY_VERSION_MINIMUM = "3.5" #Needed?
# VNL calls CMP0033; OLD is banned in CMake 4.x
#$env:CMAKE_POLICY_DEFAULT_CMP0033 = "NEW"
# (Optional) Quiet developer warnings in configure output. Not working here, moved to cxComponents
#$env:CMAKE_SUPPRESS_DEVELOPER_WARNINGS = "1"


# ------------------------------------------------------------
# 6) Run cxInstaller.py (superbuild)
# ------------------------------------------------------------
#if (-not (Test-Path ".\install\cxInstaller.py")) {
#    Write-Warning "cxInstaller.py not found in .\install. Run this script from the CustusX repo root."
#    exit 1
#}

# IMPORTANT: If you previously configured with a different generator/toolchain,
# clean the superbuild's build folders to avoid stale caches before proceeding.

#Write-Host "Running cxInstaller.py with Ninja ..."
#$cxArgs = @(
#    "--full",
#    "--all",
#    "--build_type", $BuildType,
#    "--user_doc"
#)

#& python ".\install\cxInstaller.py" $cxArgs

#if ($LASTEXITCODE -ne 0) {
#    Write-Warning "cxInstaller.py returned exit code $LASTEXITCODE"
#    exit $LASTEXITCODE
#}

