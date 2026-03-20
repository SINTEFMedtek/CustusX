# Creates a TotalSegmentator venv with GPU-enabled PyTorch

$ErrorActionPreference = "Stop"

# ---------------------------
# 1) Paths
# ---------------------------
$rootBase  = Join-Path $HOME "Fraxinus_settings\virtualEnvironments"
$toolBase  = Join-Path $rootBase "TotalSegmentator"
$segDir    = Join-Path $toolBase "segmentations"
$venvDir   = Join-Path $toolBase "venv"

# ---------------------------
# 2) (Re)create directory tree
# ---------------------------
if (Test-Path $toolBase) {
    Write-Host "Removing existing TotalSegmentator folder..."
    Remove-Item -Recurse -Force $toolBase
}
New-Item -ItemType Directory -Force -Path $segDir | Out-Null

# ---------------------------
# 3) Detect Python
# ---------------------------
if (Get-Command python.exe -ErrorAction SilentlyContinue) {
    $Python = "python.exe"
} elseif (Get-Command py.exe -ErrorAction SilentlyContinue) {
    $Python = "py.exe -3"
} else {
    throw "Python not found on PATH"
}

Write-Host "Using Python: $Python"

# ---------------------------
# 4) Create venv
# ---------------------------
Write-Host "Creating virtual environment..."
& $Python -m venv $venvDir

$VenvPython = Join-Path $venvDir "Scripts\python.exe"

# ---------------------------
# 5) Upgrade pip
# ---------------------------
Write-Host "Upgrading pip..."
& $VenvPython -m pip install --upgrade pip

# ---------------------------
# 6) Install CUDA-enabled PyTorch
# ---------------------------
Write-Host "Installing CUDA-enabled PyTorch (GPU support)..."

# For CUDA 11.8 wheels (works on most Windows GPUs, safe default)
& $VenvPython -m pip install torch --index-url https://download.pytorch.org/whl/cu118

# ---------------------------
# 7) Install TotalSegmentator
# ---------------------------
Write-Host "Installing TotalSegmentator..."
& $VenvPython -m pip install TotalSegmentator

# ---------------------------
# 8) Download weights
# ---------------------------
Write-Host "Downloading TotalSegmentator weights..."
& $VenvPython -m totalsegmentator.download_weights -t total
& $VenvPython -m totalsegmentator.download_weights -t lung_vessels
& $VenvPython -m totalsegmentator.download_weights -t lung_nodules

# ---------------------------
# 9) Summary
# ---------------------------
Write-Host ""
Write-Host "==============================================="
Write-Host "  TotalSegmentator GPU environment ready"
Write-Host "  Location:"
Write-Host "    $toolBase"
Write-Host ""
Write-Host "Activate in PowerShell:"
Write-Host "  $venvDir\Scripts\Activate.ps1"
Write-Host ""
Write-Host "Test GPU with:"
Write-Host '  python -c "import torch; print(torch.cuda.is_available())"'
Write-Host "==============================================="