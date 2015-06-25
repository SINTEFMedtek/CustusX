# This is the CustusX development installer.

$psScriptRoot = Split-Path -Path $MyInvocation.MyCommand.Definition
Push-Location $psScriptRoot
. ./Config.ps1
. ./Utilities.ps1
. ./Tool.ps1
. ./GetTools.ps1
Pop-Location

#Define which functions to make available
Export-ModuleMember -Function @(
    "Get-Tools",
	"Install-SSHKey"
    )