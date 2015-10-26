$scriptpath = $MyInvocation.MyCommand.Path

<#
.SYNOPSIS
Measure the time it took to preform a task

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 04.09.2014

.EXAMPLE
Measure-Time { jom -j8 cxResource }
#>
Function Measure-Time ($executable) {
    $sw = [Diagnostics.Stopwatch]::StartNew()
    &$executable
    $sw.Stop()
    $sw.Elapsed
}

<#
.SYNOPSIS
Check if a network drive is mapped to the system or not.

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 24.09.2012

.EXAMPLE
Test-MappedNetworkDrive "\\medtek.sintef.no\MedTekDisk"
#>
Function Test-MappedNetworkDrive{
    param(
    ## The network drive to check if is mounted
    $network_path
    )
    $found = $false
    $connections = get-wmiobject win32_networkconnection
    foreach($connection in $connections){
        if($connection.RemoteName -eq $network_path){
            $found = $true
            break
        }
    }
    return $found
}

<#
.SYNOPSIS
Get a free drive letter.

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 20.09.2012

.EXAMPLE
Get-FreeDriveLetter
#>
Function Get-FreeDriveLetter{
    foreach ($letter in [char[]]([char]'Z'..[char]'A')) {
        $drive = $letter + ‘:’
        if (!(Test-Path -path $drive)){
            break
        }
    }
    $letter
}

<#
.SYNOPSIS
Map a network drive to a free drive letter.

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 20.09.2012

.EXAMPLE
Mount-NetworkDrive "\\medtek.sintef.no\MedTekDisk"
Mounts the drive temporarly without a user.

.EXAMPLE
Mount-NetworkDrive "\\medtek.sintef.no\MedTekDisk" -username "user" -password "pass" -save True
Mounts the drive permanently logged in as a user.
#>
Function Mount-NetworkDrive{
    param(
    ## The path to the disk that should be mounted
    [parameter(Mandatory=$true, Position=0)]
    $diskLocation,
    ## The letter to assign to the remote drive
    [parameter(Mandatory=$false)]
    [char]$driveLetter,
    ## Store the mapping persistently in the users profile, default = false
    [parameter(Mandatory=$false)]
    [bool]$save=$false,
    ## If you want to log on using a specific user
    [parameter(Mandatory=$false)]
    [string]$username,
    ## Users password
    [parameter(Mandatory=$false)]
    [string]$password
    )
    if(Test-MappedNetworkDrive $diskLocation)
    {return "Not mapping $diskLocation, already mounted."}
    
    if(-not $driveLetter){
        $driveLetter = Get-FreeDriveLetter
    }
        
    $net = new-object -ComObject WScript.Network
    if($username -and $password){
        $net.MapNetworkDrive($driveLetter+":", $diskLocation, $save, $username, $password)
    }else{
        $net.MapNetworkDrive($driveLetter+":", $diskLocation, $save)
    }
}

<#
.SYNOPSIS
Checks if a command exists in the current session.

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 01.04.2012

.EXAMPLE
Command-Exists git
Checks if the command git exists in the current session.
#>
Function Command-Exists ($commandname) {
    
    if (Get-Command $commandname -errorAction SilentlyContinue)
        {return $true}
    else
        {return $false}
}

<#
.SYNOPSIS
Removes a path from the environment for this session only

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 15.11.2012

.EXAMPLE
Add-ToPathSession "C:\Path"
Will remove C:\Path\ from the current session.
#>

Function Remove-FromPathSession{
    param(
    ## The path to remove
    $path
    )
    if($env:Path.Contains($path)){
        $env:Path = $env:Path.Replace($path,'')
        Add-Logging 'SUCCESS' ("Removed "+$path+" from session path!")
    }else{
        Add-Logging 'ERROR' ("Could not find and remove "+$path+" from session path.")
    }
}
<#
.SYNOPSIS
Adds a path to the environment for this session only

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 01.04.2012

.EXAMPLE
Add-ToPathSession "C:\Path\"
Will make the C:\Path\ content reachable in the current session.
#>
Function Add-ToPathSession{
    param(
    ## The path to add
    $path
    )
    
    $env:path = $env:path + ";" + $path
    Add-Logging 'SUCCESS' ("Added "+$path+" to session!")
}

<#
.SYNOPSIS
Adds a path permanently to the system environment

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 01.04.2012

.EXAMPLE
Add-ToPathSession "C:\PathToExe\Test.exe"
Will make the Test.exe reachable permanently.
#>
Function Add-ToPathPermanent{
    param(
    ## The path to add
    $path
    )
    
    [System.Environment]::SetEnvironmentVariable("PATH", $Env:Path + ";" + $path, "Machine")
    Add-Logging 'SUCCESS' ("Added "+$tool.get_name()+" to path!")
}

<#
.SYNOPSIS
Un-/pins a file to the users taskbar

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 01.04.2012

.EXAMPLE
Toggle-PinTo-Taskbar "C:\PathToExe\Test.exe"
Will toggle Test.exe to the taskbar.
#>
function Set-PinTo-Taskbar
{
  param([parameter(Mandatory = $true)]
        [string]$application
    )
 
  $al = $application.Length
  $appfolderpath = $application.SubString(0, $al - ($application.Split("\")[$application.Split("\").Count - 1].Length))
 
  $objshell = New-Object -ComObject "Shell.Application"
  $objfolder = $objshell.Namespace($appfolderpath)
  $appname = $objfolder.ParseName($application.SubString($al - ($application.Split("\")[$application.Split("\").Count - 1].Length)))
  $verbs = $appname.verbs()
 
  foreach ($verb in $verbs)
  {
    if ($verb.name -match "(&K)")
    {
      $verb.DoIt()
    }
  }
}

<#
.SYNOPSIS
Removes personal ssh-keys that where installed by
the installer script and changes CustusX3 git protocol
to be https.

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 31.08.2012

.EXAMPLE
ConvertTo-DeveloperMachine
#>
Function ConvertTo-DeveloperMachine{
    #Remove private ssh-keys
    $ssh_dir = "~/.ssh"
    if(Test-Path "$ssh_dir")
        {rmdir $ssh_dir -Recurse}

    $dir = Split-Path $scriptpath
    if(Test-Path "$dir\id_rsa.pub")
        {Remove-Item "$dir\id_rsa.pub" -Force}
    if(Test-Path "$dir\id_rsa")
        {Remove-Item "$dir\id_rsa" -Force}
    if(Test-Path "$dir\known_hosts")
        {Remove-Item "$dir\known_hosts" -Force}

    #Change git protocol to https to enable promting every time one wants to push
    $git_config = "$script:CX_WORKSPACE\CustusX3\CustusX3\.git\config"
    (Get-Content $git_config) | 
    ForEach-Object {$_ -replace "git@github.com:", "https://github.com/"} | 
    Set-Content $git_config -Verbose
    
    #Change git config names
    Add-ToPathSession "$script:CX_PROGRAM_FILES_X86\Git\cmd"
    git config --global user.name "Developer"
    git config --global user.email "developer@sintef.no"
    
    Add-Logging 'EMPHASIS' "Machine converted to developer machine."
}

<#
.SYNOPSIS
    Invokes a command and imports its environment variables.

.DESCRIPTION
    It invokes any cmd shell command (normally a configuration batch file) and
    imports its environment variables to the calling process. Command output is
    discarded completely. It fails if the command exit code is not 0. To ignore
    the exit code use the 'call' command.
    
    http://stackoverflow.com/questions/4384814/how-to-call-batch-script-from-powershell/4385011#4385011

.EXAMPLE
    Invoke-Environment '"C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x86'

.EXAMPLE
    # Invokes Config.bat in the current directory or the system path
    Invoke-Environment Config.bat

.EXAMPLE
    # Visual Studio environment: works even if exit code is not 0
    Invoke-Environment 'call "%VS100COMNTOOLS%\vsvars32.bat"'

.EXAMPLE
    # This command fails if vsvars32.bat exit code is not 0
    Invoke-Environment '"%VS100COMNTOOLS%\vsvars32.bat"'
#>
Function Invoke-Environment{
    param
    (
        [Parameter(Mandatory=$true)] [string]
        # Any cmd shell command, normally a configuration batch file.
        $command
    )
    cmd /c "$command > nul 2>&1 && set" | .{process{
        if ($_ -match '^([^=]+)=(.*)') {
            [System.Environment]::SetEnvironmentVariable($matches[1], $matches[2])
        }
    }}

    if ($LASTEXITCODE) {
        throw "Command '$command': exit code: $LASTEXITCODE"
    }
}

<#
.SYNOPSIS
    Resets the current sessions environment.
    Affects the following environment variables:
    -Path
    -QtDir
    -QMakeSpec

.NOTES
    AUTHOR: Janne Beate Bakeng, SINTEF
    DATE: 07.09.2012

.EXAMPLE
    Clear-PSSessionEnvironment
    Clears $env:Path, $env:QtDir and $env:QMakeSpec
#>
Function Clear-PSSessionEnvironment{
    $value = [System.Environment]::GetEnvironmentVariable("PATH", "machine")
    Set-Item -Path env:path -Value $value
    Set-Item -Path env:qtdir -Value ""
    Set-Item -Path env:qmakespec -Value ""

    Add-Logging 'SUCCESS' "Cleared sessions environment (Path, QtDir and QMakeSpec, but NOT vcvarsall!)."
}

<#
.SYNOPSIS
    Sets up the environment to be ready to build 64bit.

.NOTES
    AUTHOR: Janne Beate Bakeng, SINTEF
    DATE: 25.09.2012
    
.EXAMPLE
    Set-64bitEnvironment
#>
Function Set-64bitEnvironment{
    Clear-PSSessionEnvironment
    Invoke-Environment $script:CX_CXVARS_64
    $Host.UI.RawUI.WindowTitle = "Powershell CX:x64"
    Add-Logging 'SUCCESS' "***** Setup CustusX 64 bit (x64) Development environment *****"
}

<#
.SYNOPSIS
    Sets up the environment to be ready to build 32bit.

.NOTES
    AUTHOR: Janne Beate Bakeng, SINTEF
    DATE: 25.09.2012
    
.EXAMPLE
    Set-32bitEnvironment
#>
Function Set-32bitEnvironment{
    Clear-PSSessionEnvironment
    Invoke-Environment $script:CX_CXVARS_86
    $Host.UI.RawUI.WindowTitle = "Powershell CX:x86"
    Add-Logging 'SUCCESS' "***** Setup CustusX 32 bit (x86) Development environment *****"
}

<#
.SYNOPSIS
Add Import-Module of a library to $PROFILE so that it will load 
every time powershell is opened.

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 06.09.2012

.EXAMPLE
Add-ImportModuleToProfile "C:\Dev\Powershell\Installer"
Powershell will now automatically import the Installer library every time it starts.
#>
Function Add-ImportModuleToProfile{
    param(
    ## Path to the folder where the *.psm1 file of your desired library can be found
    [Parameter(Mandatory=$true, Position=0)]
    [ValidateScript({Test-Path $_})]
    [string]$modulepath
    )
    $command = "Import-Module $modulepath -Verbose -Force"
    if(!(Select-String -Quiet -SimpleMatch $command -Path $PROFILE)){
        Add-Content -Path $PROFILE -Value $command
    }
}

# AUTHOR: Janne Beate Bakeng, SINTEF
# DATE: 04.09.2012
#
# EXAMPLES OF USAGE
#$log = New-Object Log("C:\Users\jbake\Desktop\log.txt")
#$log.addSUCCESS("Test1")
#$log.addERROR("Test2")
#$log.addWARNING("Test3")
#$log.addINFO("Test4")
#$log.addDEBUG("Test5")
#$log.print()
$logType = @'
using System;
using System.IO;
using System.Collections.Generic;

public class Log
{
    public Log(string filepath){
        if(!File.Exists(filepath)){
            string dirname = Path.GetDirectoryName(filepath);
            if(!Directory.Exists(dirname)){
                Directory.CreateDirectory(dirname);
            }
            File.CreateText(filepath).Close();
        }
        filename = filepath;
        add("","\n");
        add("[START LOGGING]    ["+getTimestamp()+"]   ","=====================================");
    }
    ~Log(){
        //No idea when this is called...
        add("[STOP LOGGING]    ["+getTimestamp()+"]   ","=====================================");
        add("","\n");
    }
    public void addSUCCESS(string message){
        add("[SUCCESS]  ["+getTimestamp()+"]   ",message);
    }
    public void addERROR(string message){
        add("[ERROR]    ["+getTimestamp()+"]   ",message);
    }
    public void addWARNING(string message){
        add("[WARNING]  ["+getTimestamp()+"]   ",message);
    }
    public void addINFO(string message){
        add("[INFO]     ["+getTimestamp()+"]   ",message);
    }
    public void addDEBUG(string message){
        add("[DEBUG]    ["+getTimestamp()+"]   ",message);
    }
    public void addHEADER(string message){
        add("******** ",message+" ********");
    }
    public void addEMPHASIS(string message){
        add("[EMPHASIS]    ["+getTimestamp()+"]   ",message);
    }
    public void print(){
        using (StreamReader r = File.OpenText(filename)){
            string line;
            while ((line = r.ReadLine()) != null)
            {
                Console.WriteLine(line);
            }
        }
    }
    
    private string getTimestamp(){
        return DateTime.Now.ToString("dd.MM.yy H:mm:ss");
    }
    private void add(string label, string message){
        string msg = label+message;
        using (StreamWriter file = new StreamWriter(filename, true)){
            file.WriteLine(msg);
        }
    }
    
    private string filename;
}
'@
Add-Type -Language CSharp -TypeDefinition $logType

<#
.SYNOPSIS
Convenience function for logging and writing to screen.

WARNING: Depends on Config.ps1

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 04.09.2012

.EXAMPLE
Add-Logging 'SUCCESS' "This worked."
#>
Function Add-Logging{
    param(
        ## The category the message should be logged as
        [Parameter(Mandatory=$true, Position=0)]
        [ValidateSet('SUCCESS','ERROR','DEBUG','INFO','WARNING','HEADER','EMPHASIS')]
        [string]$type,
        ## The message to log
        [Parameter(Mandatory=$true, Position=1)]
        [string]$message
    )
    if(!$script:CX_LOGGER){return "ERROR COULD NOT FIND LOGGER!!!"}
    switch($type){
        'SUCCESS'{$script:CX_LOGGER.addSUCCESS($message); Write-Host $message -ForegroundColor "Green"}
        'ERROR'{$script:CX_LOGGER.addERROR($message); Write-Host $message -ForegroundColor "Red"}
        'DEBUG'{$script:CX_LOGGER.addDEBUG($message); if($script:CX_DEBUG_SCRIPT){Write-Host $message -ForegroundColor "DarkGray"}}
        'INFO'{$script:CX_LOGGER.addINFO($message); Write-Host $message -ForegroundColor "White"}
        'WARNING'{$script:CX_LOGGER.addWARNING($message); Write-Host $message -ForegroundColor "DarkRed"}
        'HEADER'{$script:CX_LOGGER.addHEADER($message); Write-Host "`n******** "$message" ********" -ForegroundColor "Blue"}
        'EMPHASIS'{$script:CX_LOGGER.addEMPHASIS($message); Write-Host $message -ForegroundColor "Magenta"}
        default{Write-Host "Could not send messageto log: `"$message`" " -ForegroundColor "Red"}
    }
}

<#
.SYNOPSIS
Create new shortcut.

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 04.09.2012

.EXAMPLE
Add-Shortcut "C:\Path\To\Shortcut.lnk" 'cmd.exe'
Creates a new shortcut that points to cmd.exe.

.EXAMPLE
Add-Shortcut "C:\Path\To\Shortcut.lnk" 'cmd.exe' "/C" "C:\Path\To\Icon.ico"
Creates a new shortcut that points at cmd.exe, sends arguments /C to cmd.exe and
adds icon Icon.ico as shortcuts icon.
#>
Function Add-Shortcut{
    param(
        ## Full path to shortcut
        [Parameter(Mandatory=$true)]
        [string]$saveAsPath,
        ## Path to shortcuts target application
        [Parameter(Mandatory=$true)]
        [string]$targetPath,
        ## Arguments for the target application
        [Parameter(Mandatory=$false)]
        [string]$arguments="",
        ## Shortcuts icon
        [Parameter(Mandatory=$false)]
        [string]$iconLocation=""
    )
    $objShell = New-Object -ComObject WScript.Shell
    $objShortCut = $objShell.CreateShortcut($saveAsPath)
    $objShortCut.IconLocation = $iconLocation
    $objShortCut.TargetPath = $targetPath
    $objShortCut.Arguments = $arguments
    $objShortCut.Save()
}

<#
.SYNOPSIS
Configures machine with given ssh keys.

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 31.08.2012

.EXAMPLE
Install-SSHKey "./my_id_rsa.pub" "./my_id_rsa" "./known_hosts" "./config"

#>
Function Install-SSHKey{
    param(
        ## Path to public key file (~/.ssh/id_rsa.pub)
        [Parameter(Mandatory=$true)]
        [ValidateScript({Test-Path $_})]
        [string]$public_key,
        
        ## Path to private key file (~/.ssh/id_rsa)
        [Parameter(Mandatory=$true)]
        [ValidateScript({Test-Path $_})]
        [string]$private_key,
        
        ## Path to known host file (~/.ssh/known_hosts)
        [Parameter(Mandatory=$true)]
        [ValidateScript({Test-Path $_})]
        [string]$known_hosts,

        ## Path to config file (~/.ssh/config)
        [Parameter(Mandatory=$true)]
        [ValidateScript({Test-Path $_})]
        [string]$config,
        
        ## Whether to append or replace existing ssh keys
        [Parameter(Mandatory=$false)]
        [bool]$append=$true
    )

    $ssh_folder = "~/.ssh"
    $sshkey_public = "/id_rsa.pub"
    $sshkey_private = "/id_rsa"
    $sshkey_known_hosts = "/known_hosts"
    $sshkey_config = "/config"
    
    if(!(Test-Path $ssh_folder)){
        mkdir $ssh_folder | Out-Null
    }
    if(-not $append){
        #replace
        Copy-Item $public_key "$ssh_folder$sshkey_public" -Force
        Copy-Item $private_key "$ssh_folder$sshkey_private" -Force
        Copy-Item $known_hosts "$ssh_folder$sshkey_known_hosts" -Force
        Copy-Item $known_hosts "$ssh_folder$sshkey_config" -Force
    }else{
        #append
        Add-Content -Path "$ssh_folder$sshkey_public" -Value (Get-Content $public_key)
        Add-Content -Path "$ssh_folder$sshkey_private" -Value (Get-Content $private_key)
        Add-Content -Path "$ssh_folder$sshkey_known_hosts" -Value (Get-Content $known_hosts)
        Add-Content -Path "$ssh_folder$sshkey_config" -Value (Get-Content $config)
    }
}

<#
.SYNOPSIS
Add new tab to the Console2 application.

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 23.08.2012

.EXAMPLE
Add-Console2Tab "MyTest" "C:\Dev\powershell-cxVars_x64.bat" "C:\Temp"
Adds a tab in Console2 which starts up cmd.exe with C:\Temp as default dir.

.EXAMPLE
Add-Console2Tab "MyTest3" "%comspec%" "C:\Temp" $true
Adds a tab in Console2 which starts up cmd.exe with C:\Temp as default dir.
This time the tabs is saved in on a user level.
#>
Function Add-Console2Tab{
    param(
        ## The title of the new tab.
        [Parameter(Mandatory=$true, Position=0)]
        [string]$TabTitle,
        
        ## The console shell to execute.
        [Parameter(Mandatory=$true, Position=1)]
        [AllowEmptyString()]
        [string]$ConsoleShell,
        
        ## The startup directory.
        [Parameter(Mandatory=$true, Position=2)]
        #[ValidateScript({Test-Path $_})]
        [string]$InitalDir,
        
        ## Wheter Console saves on user or on system.
        [Parameter(Mandatory=$false, Position=3)]
        [bool]$SaveSettingsToUserDir = $false, #This is default behavior of Console2
        
        ## Path to the icon to be used
        [Parameter(Mandatory=$false)]
        [string]$Icon = ""
    )

    $console_xml = (Get-ChildItem -Path "\*\Console2\console.xml" -Recurse).fullname
    if($SaveSettingsToUserDir)
        {$console_xml ="$HOME\AppData\Roaming\Console\console.xml"}
    
    if(!(Test-Path $console_xml)) {return "Could not find: $console_xml"}

    $doc = [xml] (Get-Content $console_xml)
    $tab = $doc.Settings.Tabs.Tab
    
    $exists = $false
    for($i=0; $i -le ($tab.Count -1); $i++){
        if($tab[$i].title -eq "$TabTitle"){$exists = $true; "Tab already exists, not adding anything."}
    }
    
    $defaultIcon = "1"
    if(($icon) -and (Test-Path $icon))
        {$defaultIcon = "0"}

    if(!$exists){
        $newTab = [xml] "
        <tab title=`"$TabTitle`" icon=`"$icon`" use_default_icon=`"$defaultIcon`">
        	<console shell=`"$ConsoleShell`" init_dir=`"$InitalDir`" run_as_user=`"0`" user=`"`"/>
        	<cursor style=`"0`" r=`"255`" g=`"255`" b=`"255`"/>
        	<background type=`"0`" r=`"0`" g=`"0`" b=`"0`">
        		<image file=`"`" relative=`"0`" extend=`"0`" position=`"0`">
        			<tint opacity=`"0`" r=`"0`" g=`"0`" b=`"0`"/>
        		</image>
        	</background>
        </tab>"
        $newNode = $doc.ImportNode($newTab.tab, $true)
        $tabs = $doc.Settings.Tabs
        $appendedNode = $tabs.AppendChild($newNode)
            
        $doc.Save($console_xml)
        Write-Host "Added tab `"$TabTitle`" to Console2." -ForegroundColor Green
    }
}

<#
.SYNOPSIS
Test if Qt is built. Checks for a file called BuildStatus.txt inside
the Qt buildfolder. This file is not made by Qt, but by a local script.

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 05.10.2012

.EXAMPLE
Test-QtBuilt "C:\Dev\external_code\Qt\Qt_4.8.1_build32_DebugAndRelease"
Returns true if BuildStatus.txt is present and contains 0.
#>
Function Test-QtBuilt{
    param(
    ## The path to the Qt build folder
    [string]$buildFolder
    )
    
    $built = $false
    $buidStatusTxt = "$buildFolder\BuildStatus.txt"
    $line = Get-Content $buidStatusTxt -TotalCount 1
    if($line -match "0")
        {$built=$true}
    return $built
}

<#
.SYNOPSIS
Test if Qt is configured. It looks for configure.cache.

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 15.08.2012

.EXAMPLE
Test-QtConfigured "C:\Dev\external_code\Qt\Qt_4.8.1_build32_DebugAndRelease"
Returns true if config.cache is present, false if not.
#>
Function Test-QtConfigured{
    param(
    ## The path to the Qt build folder
    [string]$buildFolder
    )
    
    $configured = $false
    $cache = $buildFolder+"\configure.cache"
    if(Test-Path $cache)
        {$configured = $true}
    
    return $configured
}

<#
.SYNOPSIS
Get the number of cores the computer has.

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 15.08.2012

.EXAMPLE
Get-Cores
#>
Function Get-Cores{
    $core_list = (Get-WmiObject -class Win32_Processor -Property "NumberOfCores" | Select-Object -Property "NumberOfCores")
    $cores = 0
    foreach($item in $core_list){$cores += $item.NumberOfCores}
    return $cores
}

<#
.SYNOPSIS
Get the computers operative architecture.

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 15.08.2012

.EXAMPLE
Get-OsArchitecture
Known return values: 32-bit, 64-bit
#>
Function Get-OsArchitecture{
    return (Get-WmiObject -Class Win32_OperatingSystem | Select-Object OSArchitecture).OSArchitecture
}

<#
.SYNOPSIS
Check if a microsoft visual studio c++ 2010 compiler
for the give architecuter is found.

WARNING: Depends on Config.ps1

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 15.08.2012

.EXAMPLE
Find-Compiler "x86"
Looks for a 32 bit compiler

.EXAMPLE
Find-Compiler "x64"
Looks for a 64 bit compiler

.EXAMPLE
Find-Compiler "x86_amd64"
Looks for a cross compiler
#>
Function Find-Compiler{
    param(
    ## The compiler architecture to look for
    [string]$arch
    )
    $found = $false
    switch ($arch){
        "x86" {$found = (Test-Path "$script:CX_MSVC_CL_X86")}
        "x64" {$found = (Test-Path "$script:CX_MSVC_CL_X64")}
        "x86_amd64" {$found = (Test-Path "$script:CX_MSVC_CL_X86_AMD64")}
        default {Write-Host "Compiler architecture $arch not recognized."}
    }
    return $found
}

<#
.SYNOPSIS
Check if a microsoft visual studio c++ 2013 is installed

WARNING: Depends on Config.ps1

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 15.08.2012

.EXAMPLE
Test-MSVCInstalled
Returns true if 32 bit compiler of mvs2013 is found, false if not.
#>
Function Test-MSVCInstalled{
    return (Test-Path $script:CX_MSVC_CL_X86)
}

<#
.SYNOPSIS
Get the elapsed time since a give input.

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 15.08.2012

.EXAMPLE

#>
Function Get-ElapsedTime{
    param(
    [System.DateTime]$startTime
    )
    $runtime = $(get-date) - $startTime
    $retStr = [string]::format("{0} days, {1} hours, {2} minutes, {3}.{4} seconds", `
        $runtime.Days, `
        $runtime.Hours, `
        $runtime.Minutes, `
        $runtime.Seconds, `
        $runtime.Milliseconds)
    $retStr
}

<#
.SYNOPSIS
Resizes the current powershell window buffer to 120x32500

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 15.08.2012

.EXAMPLE
Expand-WindowBuffer
Resized the powershells window buffer to 120x32500
#>
Function Expand-WindowBuffer{
    $pshost = Get-Host
    $pswindow = $pshost.ui.rawui

    $newsize = $pswindow.buffersize
    $newsize.height = 32500
    $newsize.width = 120
    $pswindow.buffersize = $newsize
}

Add-Type -AssemblyName System.Drawing #for function Export-Icon

<#
.SYNOPSIS
Exports a executables icon.

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 15.08.2012

.EXAMPLE
Export-Icon "C:\Application.exe" "C:\Temp\Icon.ico"
Export Application.exe's icon and saves it as Icon.ico
#>
Function Export-Icon{
    param(
    ## Path to the executable with icon to extract
    [string]$exeName,
    ## Full path to file where icon should be saved
    [string]$saveAs
    )
    $stream = [System.IO.File]::OpenWrite($saveAs)
    $icon = [Drawing.Icon]::ExtractAssociatedIcon((Get-Command $exeName).Path)
    $icon.Save($stream)
    $stream.Close()
}

<#
.SYNOPSIS
Copies environment variables from batchfile back to
powershell session.

http://www.tavaresstudios.com/Blog/post/The-last-vsvars32ps1-Ill-ever-need.aspx

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 15.10.2014

.EXAMPLE
Get-Batchfile vcvars32
Copies all visual studio environment variables back to
the powershell session.
#>
function Get-Batchfile ($file) {
    $cmd = """$file"" & set"
    cmd /c $cmd | Foreach-Object {
        $p, $v = $_.split('=')
        if($v -and $p) {
            Set-Item -path env:$p -value $v
        }
    }
}