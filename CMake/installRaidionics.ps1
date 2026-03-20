$ErrorActionPreference = "Stop"

#############################################################
# 1) CREATE / RECREATE PYTHON VENV
#############################################################

$venvBase  = Join-Path $HOME "Fraxinus_settings\virtualEnvironments"
$venvPath  = Join-Path $venvBase "raidionicsVenv"

# Delete old venv
if (Test-Path $venvPath) {
    Write-Host "Removing existing Python environment..."
    Remove-Item -Recurse -Force $venvPath
}

# Ensure base folder exists
New-Item -ItemType Directory -Force -Path $venvBase | Out-Null

# Detect Python
if (Get-Command python.exe -ErrorAction SilentlyContinue) {
    $Python = "python.exe"
} elseif (Get-Command py.exe -ErrorAction SilentlyContinue) {
    $Python = "py.exe -3"
} else {
    throw "Python not found on PATH"
}

Write-Host "Using Python: $Python"

# Create venv
Write-Host "Creating virtual environment..."
& $Python -m venv $venvPath

# venv Python
$VenvPython = Join-Path $venvPath "Scripts\python.exe"

Write-Host "Upgrading pip..."
& $VenvPython -m pip install --upgrade pip

Write-Host "Installing raidionics-rads-lib v1.2.0..."
& $VenvPython -m pip install git+https://github.com/dbouget/raidionics-rads-lib.git@v1.2.0

Write-Host "Installing onnxruntime-gpu==1.23..."
& $VenvPython -m pip install onnxruntime-gpu==1.23


#############################################################
# 2) DOWNLOAD & UNZIP RAIDIONICS MODELS
#############################################################

$modelsBase = Join-Path $HOME "Fraxinus_settings\models\raidionics_models"

New-Item -Path $modelsBase -ItemType Directory -Force | Out-Null
Set-Location -Path $modelsBase

# Ensure TLS 1.2+
[Net.ServicePointManager]::SecurityProtocol =
    [Net.SecurityProtocolType]::Tls12 -bor
    [Net.SecurityProtocolType]::Tls13

$repoPath = "https://github.com/raidionics/Raidionics-models/releases/download/v1.3.0-rc/"

$models = @(
    "Raidionics-CT_Airways-v13.zip",
    "Raidionics-CT_Lungs-v13.zip",
    "Raidionics-CT_LymphNodes-v13.zip",
    "Raidionics-CT_MediumOrgansMediastinum-v13.zip",
    "Raidionics-CT_PulmSystHeart-v13.zip",
    "Raidionics-CT_SmallOrgansMediastinum-v13.zip",
    "Raidionics-CT_Tumor-v13.zip"
)

foreach ($model in $models) {
    $url     = $repoPath + $model
    $zipPath = Join-Path $modelsBase $model

    Write-Host "Checking $model..."

    $shouldDownload = $true

    if (Test-Path $zipPath) {
        try {
            $req = [System.Net.HttpWebRequest]::Create($url)
            $req.Method = "HEAD"
            $resp = $req.GetResponse()
            $remoteLen = $resp.Headers["Content-Length"]
            $resp.Close()

            if ($remoteLen -and (Get-Item $zipPath).Length -eq [int64]$remoteLen) {
                Write-Host "Already up to date: $model"
                $shouldDownload = $false
            }
        } catch {
            $shouldDownload = $true
        }
    }

    if ($shouldDownload) {
        Write-Host "Downloading $model..."
        Invoke-WebRequest -Uri $url -OutFile $zipPath -UseBasicParsing
    }

    Write-Host "Unzipping $model..."
    try {
        Expand-Archive -Path $zipPath -DestinationPath $modelsBase -Force
    } catch {
        Write-Warning "Error unzipping ${model}: $($_.Exception.Message)"
    }
}

Write-Host ""
Write-Host "==============================================="
Write-Host "  INSTALLATION COMPLETE"
Write-Host "==============================================="
Write-Host ""
Write-Host "Python environment:"
Write-Host "  $venvPath"
Write-Host ""
Write-Host "Models downloaded to:"
Write-Host "  $modelsBase"
Write-Host ""
Write-Host "Activate venv in PowerShell:"
Write-Host "  $venvPath\Scripts\Activate.ps1"
Write-Host ""
Write-Host "Activate venv in CMD:"
Write-Host "  $venvPath\Scripts\activate.bat"
Write-Host ""
Write-Host "Deactivate:"
Write-Host "  deactivate"