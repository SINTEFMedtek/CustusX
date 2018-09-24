<#
.SYNOPSIS
Script that prepares a Windows machine for software development.

.DESCRIPTION
Downloads, installs and sets up environment for:
-Microsoft Visual Studio Community 2015
-7-zip
-CppUnit
-ninja
-git
-CMake
-Python
-Eclipse
-Qt
-Boost

.INPUTS
None. You cannot pipe to this script.

.OUTPUTS
None. This script does not generate any output.

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 21.05.2012


#>

# Import other scripts
############################
$psScriptRoot = Split-Path -Path $MyInvocation.MyCommand.Definition
Push-Location $psScriptRoot
. .\Utilities.ps1
. .\Config.ps1
. .\Tool.ps1
Pop-Location


<#
.SYNOPSIS
Checks if a tool exists in the current session.

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 01.04.2012

.EXAMPLE
Tool-Exists $git
Checks if the tool exists in the current session.
#>
Function Tool-Exists{
    param(
    ## The tool we want to check if exists
    $tool
    )
    $exists = $false
    
    if(($tool.get_name() -eq "cppunit") -or ($tool.get_name() -eq "boost") -or ($tool.get_name() -eq "glew")){
        if(Test-Path $tool.get_installedBinFolder())
            {$exists = $true}
    }elseif($tool.get_executableName() -and (Command-Exists $tool.get_executableName()))
        {$exists = $true}
    elseif($tool.get_name() -eq "MSVC2015"){$exists=Test-MSVCInstalled}
    
    if($exists -eq $true)
        {Add-Logging 'INFO' ($tool.get_name()+" already exists")}
        
    return $exists
}


<#
.SYNOPSIS
Tries to download a tools installer file.

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 01.04.2012

.EXAMPLE
Download-Tool $git
Tries to download gits installer file.
#>
Function Download-Tool{
    param(
    ## The tool we want to download the installer file for.
    $tool
    )
    $success = $false
    if($tool.get_name() -eq "ninja") {
        return $true
    }
    try{
        Add-Logging 'INFO'  ("Downloading "+$tool.get_name())
        $webclient = New-Object Net.WebClient
        $webclient.DownloadFile($tool.get_downloadUrl(), $tool.get_saveAs())
        Add-Logging 'INFO' "Download done."
        $success = $true
    }
    catch
    {
        Add-Logging 'ERROR' ("Exception caught when trying to download "+$tool.get_name()+" from "+$url+" to "+$targetFile+".")
    }
    finally
    {
        if($success)
            {Add-Logging 'SUCCESS' ("Downloaded "+$tool.get_name()+"!")}
        else
            {Add-Logging 'ERROR' ("Could not download "+$tool.get_name()+", you will have to do it manually!")}
    }
    return $success
}

<#
.SYNOPSIS
Tries to install a tool.

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 01.04.2012

.EXAMPLE
Install-Tool $git
Tries to install git.
#>
Function Install-Tool{
    param(
    ## The tool we want to install.
    $tool
    )
    $success = $false 

    $success = Run-Installer $tool
    
    if($success){
        Add-Logging 'SUCCESS' ("Installed "+$tool.get_name()+"!")
        if($tool.get_name() -like 'console2'){
        $toolname = $tool.get_name()
            $icon = $script:CX_ENVIRONMENT_FOLDER+"\"+$toolname+".ico"
            Export-Icon $tool.get_executableName() $icon
            Add-Shortcut ("$HOME\Desktop\$toolname.lnk") ("`""+$tool.get_installedBinFolder()+"\"+$tool.get_executableName()+".exe`"") "" "$icon"
        }
    }
    else
        {Add-Logging 'ERROR' ("Could not install "+$tool.get_name()+", you will have to do it manually!")}
        
    return $success
}

<#
.SYNOPSIS
Runs the tools installer.

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 01.04.2012

.EXAMPLE
Run-Installer $git
Runs the downloaded git installer.
#>
Function Run-Installer{
    param(
    ## The tool we want to run the installer for.
    $tool
    )

    Add-Logging 'INFO' ("Installing "+$tool.get_name())
    
    $success = $false
    $packageType = $tool.get_packageType()
    if($packageType -eq "NSIS package"){
        #piping to Out-Null seems to by-pass the UAC
        Start-Process $tool.get_saveAs() -ArgumentList "/S" -NoNewWindow -Wait | Out-Null
        $success = $true    
    }
    elseif($packageType -eq "Inno Setup package"){
        #piping to Out-Null seems to by-pass the UAC
        Start-Process $tool.get_saveAs() -ArgumentList "/VERYSILENT /SUPPRESSMSGBOXES /NORESTART /SP-" -NoNewWindow -Wait | Out-Null
        $success = $true
    }
    elseif($packageType -eq "MSI"){
        $installer = $tool.get_saveAs()
        Start-Process msiexec -ArgumentList "/i $installer /quiet /passive" -NoNewWindow -Wait
        $success = $true
    }
    elseif($packageType -eq "ZIP"){
        $shell_app = new-object -com shell.application
        $zip_file = $shell_app.namespace($tool.get_saveAs())

        $destinationFolder = $tool.get_extractFolder()
        if(!(Test-Path $destinationFolder))
            {mkdir $destinationFolder}
        $destination = $shell_app.namespace($destinationFolder)
        $destination.Copyhere($zip_file.items(),0x14) #0x4 hides dialogbox, 0x10 overwrites existing files, 0x14 combines both
        
        $success = $true
    }
    elseif($packageType -eq "TarGz"){
        $z ="7z.exe"

        #Destination folder cannot contain spaces for 7z to work with -o
        $destinationFolder = $tool.get_extractFolder()
        if(!(Test-Path $destinationFolder))
            {mkdir $destinationFolder}
        Add-Logging 'DEBUG' ('$destinationFolder '+$destinationFolder)
         
        $targzSource = $tool.get_saveAs()
        Add-Logging 'DEBUG' ('$targzSource '+$targzSource)
        & "$z" x -y $targzSource #| Out-Null
        
        $tarSource = (Get-Item $targzSource).basename
        Add-Logging 'DEBUG' ('$tarSource '+$tarSource)
        & "$z" x -y $tarSource "-o$destinationFolder" #| Out-Null #Need to have double quotes around the -o parameter because of whitespaces in destination folder
   
        Remove-Item $tarSource
        
        $success = $true
    }
    elseif($packageType -eq "EXE"){
        #Made to work with the Microsoft Visual Studio 2010 Express C++ web installer
        Start-Process $tool.get_saveAs() -ArgumentList "/q /norestart" -NoNewWindow -Wait | Out-Null
        $success = $true
    }
    elseif($packageType -eq "UPX Packed executable"){
        #Made to work with Firefox.exe installer
        Start-Process $tool.get_saveAs() -ArgumentList "-ms" -NoNewWindow -Wait | Out-Null
        $success = $true
    }
	elseif($packageType -eq "GitBootstrap"){
		$argument = "clone "+$tool.get_downloadUrl()+" "+$tool.get_installedBinFolder()+""
		Start-Process "git" -ArgumentList $argument -NoNewWindow -Wait | Out-Null
		
		Push-Location -Path $tool.get_installedBinFolder()
		Get-Batchfile $script:CX_MSVC_VCVARSALL
		Start-Process "python" -ArgumentList @("configure.py", "--bootstrap") -Wait -NoNewWindow
		Add-Logging 'DEBUG' ('Done bootstraping')
		Pop-Location

        $success = $true
    }
	elseif($packageType -eq "CUDAInstaller"){
		#NOTE this might restart the windows... that's an issue...
		#http://docs.nvidia.com/cuda/cuda-getting-started-guide-for-microsoft-windows/#axzz3dsO4y2xl
		Start-Process $tool.get_saveAs() -ArgumentList "-s" -Wait -NoNewWindow
		$success = $true
	}
    elseif($packageType -eq ""){
        #If no package type is specified, assume no installation is needed
        #Made to work with ussf.exe
        $success = $true
    }
    else{
        Add-Logging 'ERROR' ("Could not figure out which installer "+$tool.get_name()+" has used, could not install "+$tool.get_name()+".")
    }

    return $success
}

<#
.SYNOPSIS
Adds a tools installed path to the system environment for this session.

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 01.04.2012

.EXAMPLE
Add-ToPathSession "C:\PathToExe\Test.exe"
Will make the Test.exe reachable in the current session.
#>
Function Add-ToPath{
    param(
    ## The tool that we want to make available.
    $tool
    )
    
    Add-Logging 'INFO' ("Adding "+$tool.get_name()+" to system environment.")

    $success = $false 
    
    $path = $tool.get_installedBinFolder()
    Add-ToPathSession($path)
    $success = $true
    
    return $success
}

# Creates a bat file that sets up a CustusX build environment
Function Create-Cx-Environment($saveName, $targetPlatform, $extendedPath){
    $qt_dir = $script:CX_QT_QTDIR_X64
    $qt_bin = $qt_dir+"\bin"
    Add-Logging 'DEBUG' ('$qt_bin '+$qt_bin)
    $extendedPath = $qt_bin+";"+$extendedPath
    Add-Logging 'DEBUG' ('$extendedPath '+$extendedPath)
    
    $content = @"
@echo off
rem
rem This file is automatically generated by the Windows installer script for CustusX
rem

echo ====================================================
echo Setting up a CustusX ($targetPlatform) environment...
echo ====================================================
echo.
echo ******* Setting up a tool enabled environment *******
:: Removing quotes from path, because jenkins node fail if something in path contains double quotes
set PATH=%PATH:"=%
set PATH=%PATH%;$extendedPath
echo.

echo ******* Setting up Microsoft Visual Studio $script:CX_MSVC_VERSION ($targetPlatform) environment *******
call "$script:CX_MSVC_VCVARSALL" $targetPlatform

"@

    $envFileFullName = $saveName

    $stream = New-Object System.IO.StreamWriter($envFileFullName)
    $stream.WriteLine($content)
    $stream.Close()
    
    Add-Logging 'SUCCESS'  "Created environment file $envFileFullName"
    
    return $envFileFullName
}

<#
.SYNOPSIS
Creates a shortcut to a batch file that run a tool within custusx environment

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 01.04.2012

.EXAMPLE
Create-Batch-Exe 'cmd' "$script:CX_CXVARS_64" "C:\Temp"
Will create a shortcut to the command executable loaded with custusX 64bit environment.
Files are saved in C:\Temp.
#>
Function Create-Batch-Exe{
    param(
    ## The executable that should be loaded in a specific environment
    $toolExecutableName,
    ## The bat file that sets up the environment
    $cxVarsFile,
    ## The folder where the files should be saved
    $saveFolder
    )
	
    if(!(Test-Path $cxVarsFile))
        {Add-Logging 'WARNING' "Cannot create batch exe because $cxVarsFile cannot be found."}
    
    #variables
    $toolName = $toolExecutableName
    if($toolName -eq "cmake"){
        $toolName = "cmake-gui"
    }
	if($toolName -eq "qmake"){
        $toolName = "qtcreator"
    }
    $cxVarsFileBase = ((Get-Item $cxVarsFile | Select-Object basename )).basename
    $batchName = "$toolName-$cxVarsFileBase"
    $batchEnding = ".bat"
    
    $batchPath = "$saveFolder\$batchName$batchEnding"
    $toolExe = (Get-Command $toolName | Select-Object Name).Name
    $toolFolder = (Get-Item (Get-Command $toolName | Select-Object Definition).Definition).directory.fullname

    $desktopFolder = "$HOME\Desktop\"
    $taskbarFolder = "$HOME\AppData\Roaming\Microsoft\Internet Explorer\Quick Launch\User Pinned\Taskbar\"
    $shortcutFolder = $desktopFolder
    
    #write content for normal executables
    $content = @"
@cd $saveFolder
@call $cxVarsFile > nul 2>&1
@cd $toolFolder
@start $toolExe > nul 2>&1
@exit
"@
    #Powershell specifics
    if($toolName -eq "powershell")
    {
        $command = "if('$cxVarsFile' -like '*x64*' ) {Write-Host '***** Setup CustusX 64 bit (x64) Development environment *****' -ForegroundColor Green; if(!(Test-Path `'$script:CX_MSVC_CL_X64`')){Write-Host 'You do NOT have a 64 bit compiler installed!' -ForegroundColor Red;}}elseif('$cxVarsFile' -like '*x86*' ) {Write-Host '***** Setup CustusX 32 bit (x86) Development environment *****' -ForegroundColor Green;};"
        $content = @"
@call $cxVarsFile > nul 2>&1
powershell -NoExit -Command "& {$command}"
"@
        if("$cxVarsFile" -like '*x64*' ){Add-Console2Tab -TabTitle "Powershell CX:x64" -ConsoleShell "$batchPath" -InitalDir "$script:CX_WORKSPACE\CX\" -SaveSettingsToUserDir $false}
    }

	write-host $batchPath
    $stream = New-Object System.IO.StreamWriter($batchPath)
    $stream.WriteLine($content)
    $stream.Close()
    
    #extract icon
    $icon = $script:CX_ENVIRONMENT_FOLDER+"\"+$toolname+".ico"
    Export-Icon $toolName $icon
    
    #create shortcut on taskbar
    $arch = "x64"
    if("$cxVarsFile" -like '*x64*' ){$arch = "x64"}
    $shortcutPath = "$shortcutFolder"+"\"+$toolName+"_"+$arch+".lnk"
    
    Add-Shortcut $shortcutPath 'cmd.exe' "/C ""$batchPath""" "$icon"
    
    Add-Logging 'SUCCESS' "Created shortcut to $toolExecutableName started in a CustusX environment!"
    
    return $true
}

<#
.SYNOPSIS
Configures the git installation.

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 01.04.2012

.EXAMPLE
Configure-Git 'Developer' 'dev@sintef.no'
Configures the git installation.
#>
Function Configure-Git{
    param(
    ## The name that commits should be signed with
    $name,
    ## The e-mail adressed used by git to associate git commits to a person
    $email
    )
    ## See https://help.github.com/articles/set-up-git
    git config --global user.name $name
    git config --global user.email $email
    git config --global credential.helper 'cache --timeout=86400' # Will only work when you clone an HTTPS repo URL. If you use the SSH repo URL instead, SSH keys are used for authentication.
    
    git config --global color.diff auto
    git config --global color.status auto
    git config --global color.branch auto
    git config --global core.autocrlf input
    git config --global core.filemode false #(ONLY FOR WINDOWS)
    
    Add-Logging 'EMPHASIS' "Configured git, see: git config --list"
}

<#
.SYNOPSIS
Creates the CustusX environment

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 05.09.2013

.EXAMPLE
Create-Environment
#>
Function Create-Environment{
    param(
    ## path to all needed tools
    $extendedPath,
    ## list of all tools that should be installed
    $SelectedTools,
    ## folder to save environment files in
    $CxEnvFolder,
    ## path to where cxVars_64.bat should be saved
    $CXVARS_64
    )
    #Create batch files for setting up the developer environment
    #--------------
    Add-Logging 'HEADER' "ENVIRONMENT"

    #Check that prerequirements are met
    if(!(Test-MSVCInstalled)){
        Add-Logging 'ERROR' "You need to have Microsoft Visual Studio 2015 installed before setting up a environment."
        return "Abort"
    }
    
    # create 64bit CustusX environment for cmd 
    Create-Cx-Environment $CXVARS_64 "x64" $extendedPath 
    
    # create shortcut that loads eclipse and cmake in correct environment
    # they will only run in a 64 bit environment
    foreach($t in $SelectedTools){
        if(($t.get_name() -eq "cmake") -or ($t.get_name() -eq "eclipse") -or ($t.get_name() -eq "qt")){
		write-host $t.get_name()
            if(!(Create-Batch-Exe $t.get_executableName() $CXVARS_64 $CxEnvFolder)){
                Add-Logging 'WARNING' ("Could not create 64 bit shortcut and batch file for "+$t.get_name())
            }
        }
        if($t.get_name() -eq "git"){
            Add-Console2Tab -TabTitle "Git Bash" -ConsoleShell "cmd.exe /c &quot;&quot;$script:CX_PROGRAM_FILES_X86\Git\bin\sh.exe&quot; --login -i&quot;" -InitalDir "$script:CX_WORKSPACE\CX" -SaveSettingsToUserDir $false -Icon "$script:CX_PROGRAM_FILES_X86\Git\etc\git.ico"
        }
    }

    # create shortcut to powershell.exe with cx environment
    $powershellExecutableName = "powershell"
    Create-Batch-Exe $powershellExecutableName $script:CX_CXVARS_64 $CxEnvFolder | Out-Null
    
    # add cmd tab to console 2
    Add-Console2Tab -TabTitle "Cmd" -ConsoleShell "cmd" -InitalDir "$script:CX_WORKSPACE" -SaveSettingsToUserDir $false
}

<#
.SYNOPSIS
Convenience function for checking out, configuring and building CustusX.

WARNING: Depends on Config.ps1

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 05.09.2012

.EXAMPLE
Get-Tools full
Get all available tools. Download, install and setup CustusX environment for them.

.EXAMPLE
Get-Tools full' -tools @('git', 'jom')
Get git and jom. Download, install and setup CustusX environment for them.

.EXAMPLE
Get-Tools download
Download tools.

.EXAMPLE
Get-Tools install
Install tools.

.EXAMPLE
Get-Tools environment
Setup CustusX environment for them.
#>
Function Get-Tools {

param (
    ## Select which mode to run in, what should be done with the selected tools
    [Parameter(Mandatory=$true, HelpMessage="Select installation mode. (full, download, install, environment")]
    [ValidateSet('full', 'download', 'install', 'environment')]
    [string[]]$mode,
    ## Manually picking tools
    [Parameter(Mandatory=$false, HelpMessage="Select tool(s). (7-Zip, cppunit, ninja, git, cmake, python, eclipse, qt, boost, MSVC2015, console2, nsis, firefox, cuda, ussf, glew)")]
    [ValidateSet('7-Zip', 'cppunit', 'ninja', 'git', 'cmake', 'python', 'eclipse', 'qt', 'boost', 'MSVC2015', 'console2', 'nsis', 'firefox', 'cuda', 'ussf', 'glew')]
    [string[]]$tools
)
#Set security protocol to be able to download using the net.webclient 
#--------------
[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12


#Information 
#--------------
    $ToolFolder = $script:CX_TOOL_FOLDER
    $CxEnvFolder = $script:CX_ENVIRONMENT_FOLDER

#Setup tool containers
#--------------
 	$AllTools = Get-ToolList
    $SelectedTools = @()
	
#Create extended path and add to session
#--------------
    foreach($t in $AllTools){
        $extendedPath = $extendedPath+";"+$t.get_installedBinFolder()+";"+$t.get_extraPaths()+""
    }

#Parse parameters
#--------------
	if($tools -ne $null) {
		foreach($selected_tool in $tools){
			foreach($available_tool in $AllTools){
				if($selected_tool -eq $available_tool.get_name()){
					$SelectedTools +=  $available_tool
				}
			}
		}
	} else {
		$SelectedTools = $AllTools
	}

#Prompt to continue
#--------------
    Add-Logging 'INFO' "You have selected the following tools:"
    foreach($t in $SelectedTools){
        Add-Logging 'INFO' ("--"+$t.get_name()+"`t`t `""+$t.get_helpText()+"`"")
    }
    Write-Host "`n"
    Add-Logging 'INFO' "You have selected the following actions:"
    if((($mode -eq "full") -or ($mode -eq "download")))
        {Add-Logging 'INFO' "-- Downloading"}
    if((($mode -eq "full") -or ($mode -eq "install")))
        {Add-Logging 'INFO' "-- Installing"}
    if((($mode -eq "full") -or ($mode -eq "environment")))
        {Add-Logging 'INFO' "-- Setting up environment"}
        
    $ready = Read-Host "`nContinue? y/n"
    if($ready -ne "y")
        {return "quit"}

#Tell the user to relax and let the script do its job
#--------------
    Add-Logging 'EMPHASIS' "Ready to run, you can go drink some coffee now. :)"
    Add-Logging 'HEADER' "PREPARATIONS"

#Adding to session path so that we can search to see if tool exists
    Add-ToPathSession $extendedPath

#Create folders
#--------------
    if((($mode -eq "full") -or ($mode -eq "download")))
     {mkdir $ToolFolder -force | Out-Null}
    if((($mode -eq "full") -or ($mode -eq "environment")))
     {mkdir $CxEnvFolder -force | Out-Null}

#Download and install tools
#--------------
    Add-Logging 'HEADER' "DOWNLOAD AND INSTALL TOOLS"
    for($i=0; $i -le $SelectedTools.Length -1;$i++)
    {
        $tool = $SelectedTools[$i]
        
        if(Tool-Exists $tool)
            {continue}
            
        Add-Logging 'INFO' ("Missing tool "+$tool.get_name())
        
        #Downloading tool
        if((($mode -eq "full") -or ($mode -eq "download")))
        {
            if(!(Download-Tool $tool))
               {continue}
        }
        #Installing tool
        if((($mode -eq "full") -or ($mode -eq "install")))
        {
            if(!(Install-Tool $tool))
                {continue}
        }
        #Add to path to make tools avaiable in this session
        if(!(Add-ToPath $tool))
            {continue}
        
        #Configure git
        if($tool.get_name() -eq "git")
            {Configure-Git $script:CX_GIT_NAME $script:CX_GIT_EMAIL}
    }

    if((($mode -eq "full") -or ($mode -eq "environment")))
    {
        Create-Environment $extendedPath $SelectedTools $CxEnvFolder $script:CX_CXVARS_64
    }

return $true
}
