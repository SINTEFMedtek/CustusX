:: ===== README =====
:: To get help on powershell functions 
:: Get-Help <Command>
:: Example:
:: Get-Help Get-Tools

:: Batch script that executes a powershell scrip that will
:: install tools for software development on Windows.
@echo off 

:: Sets the execution policy for powershell
powershell -command "& {Set-ExecutionPolicy Unrestricted}"

:: Imports the installer module and runs the Setup function
:: which will setup a CustusX developer environment
powershell -noprofile -command "&{"^
 "Import-Module ..\Setup -Force;"^
 "Get-Tools 'full' -tools @('console2', 'MSVC2013', '7-Zip', 'cppunit', 'git' , 'python', 'ninja', 'cmake', 'nsis', 'ussf', 'cuda', 'boost');"^
 "Get-Tools 'full' -tools 'qt';"^
 "(Invoke-WebRequest - UseBasicParsing https://bootstrap.pypa.io/ez_setup.py).Content | python -;"^
 "}"
