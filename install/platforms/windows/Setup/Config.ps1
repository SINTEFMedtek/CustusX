# Help
#############################################################################

<#
.SYNOPSIS
This file contains globals for the other scripts
in the Windows installer

.DESCRIPTION


.INPUTS
None. You cannot pipe to this script.

.OUTPUTS
None. This script does not generate any output.

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 21.08.2012

#>
$psScriptRoot = Split-Path -Path $MyInvocation.MyCommand.Definition
Push-Location $psScriptRoot
. .\Utilities.ps1
Pop-Location

# PRIVATE user information 
# (EDIT)
###################################################
$script:CX_GIT_NAME = "YOUR_NAME"
$script:CX_GIT_EMAIL = "your_name@domain.com"


# System information 
# (normally no need to edit)
###################################################
$script:CX_DEBUG_SCRIPT = $true #use if developing the windows installer script

$script:CX_DEFAULT_DRIVE = "C:" #This should be the drive where windows and all your software is installed
$script:CX_PROGRAM_FILES = $CX_DEFAULT_DRIVE+"\Program Files"
$script:CX_PROGRAM_FILES_X86 = $CX_DEFAULT_DRIVE+"\Program Files (x86)"

$script:CX_MSVC_VERSION = "14.0"
$script:CX_MSVC = $CX_PROGRAM_FILES_X86+"\Microsoft Visual Studio $CX_MSVC_VERSION"
$script:CX_MSVC_CL_X86 = $CX_MSVC+"\VC\bin\cl.exe"
$script:CX_MSVC_CL_X64 = $CX_MSVC+"\VC\bin\amd64\cl.exe"
$script:CX_MSVC_CL_X86_AMD64 = $CX_MSVC+"\VC\bin\x86_amd64\cl.exe"
$script:CX_MSVC_VCVARSALL = $CX_MSVC+"\VC\vcvarsall.bat"

$script:CX_ROOT = $CX_DEFAULT_DRIVE+"\Dev"
$script:CX_WORKSPACE = $CX_ROOT+"\cx"
$script:CX_EXTERNAL_CODE = $script:CX_WORKSPACE

$script:CX_QT_VERSION = "5.6"
$script:CX_QT_BUILD_X64 = $script:CX_DEFAULT_DRIVE+"\Qt\Qt"+$CX_QT_VERSION+"\5.6\msvc2015_64\"
$script:CX_QT_QTDIR_X64 = $script:CX_QT_BUILD_X64

$script:CX_CORES = Get-Cores

$script:CX_TOOL_FOLDER = "$script:CX_WORKSPACE\Downloaded_applications"
$script:CX_ENVIRONMENT_FOLDER = "$script:CX_WORKSPACE\CustusX_environment"
$script:CX_CXVARS_64 = $CX_ENVIRONMENT_FOLDER+"\cxVars_x64.bat"

if(!$script:CX_LOGGER){
    $script:CX_LOGGER = New-Object Log($script:CX_ENVIRONMENT_FOLDER+"\Installation_log.txt")
}