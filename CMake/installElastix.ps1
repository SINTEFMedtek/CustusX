# Minimal installer for your ZIP layout
$ZipUrl = "https://github.com/SuperElastix/elastix/releases/download/5.3.0/elastix-5.3.0-windows.zip"
$ZipPath = "$env:TEMP\elastix.zip"
$InstallDir = "C:\Elastix"

Invoke-WebRequest -Uri $ZipUrl -OutFile $ZipPath
Expand-Archive -Path $ZipPath -DestinationPath $InstallDir -Force

$binPath = "C:\Elastix"
$userPath = [System.Environment]::GetEnvironmentVariable("Path","User")

if ($userPath -notlike "*$binPath*") {
    $newPath = $userPath + ";" + $binPath
    [System.Environment]::SetEnvironmentVariable("Path", $newPath, "User")
    Write-Host "Added Elastix to USER PATH. Restart your terminal."
} else {
    Write-Host "Elastix already in USER PATH."
}