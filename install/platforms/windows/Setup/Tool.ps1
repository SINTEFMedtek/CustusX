<#
.SYNOPSIS
This file defines all availble tools

.DESCRIPTION

.INPUTS
None. You cannot pipe to this script.

.OUTPUTS
None. This script does not generate any output.

.NOTES
AUTHOR: Janne Beate Bakeng, SINTEF
DATE: 15.10.2014

#>

# Classes
#############################################################################

$toolType = @'
public class Tool{
    public Tool(
        string name, 
        string downloadUrl, 
        string saveAs, 
        string packageType, 
        string installedBinFolder, 
        string extraPaths,
        string extractFolder, 
        string executableName,
        string helpText
        )
    {
        mName = name;
        mDownloadUrl = downloadUrl;
        mSaveAs = saveAs;
        mPackageType = packageType;
        mInstalledBinFolder = installedBinFolder;
        mExtraPaths = extraPaths;
        mExtractFolder = extractFolder;
        mExecutableName = executableName;
        mHelpText = helpText;
    }
    
    public string get_name(){ return mName;}
    public string get_downloadUrl(){ return mDownloadUrl;}
    public string get_saveAs(){ return mSaveAs;}
    public string get_packageType(){ return mPackageType;}
    public string get_installedBinFolder(){ return mInstalledBinFolder;}
    public string get_extraPaths(){ return mExtraPaths;}
    public string get_extractFolder(){ return mExtractFolder;}
    public string get_executableName(){ return mExecutableName;}
    public string get_helpText(){ return mHelpText;}
    
    private string mName; //name of the tool
    private string mDownloadUrl; //the url to the downloadable file
    private string mSaveAs; //what the downloaded file should be saved as
    private string mPackageType; //the download file type
    private string mInstalledBinFolder; //where the executable can be found after tool is installed
    private string mExtraPaths; //for some tools more than the installedBinFolder needs to be added to the environment path
    private string mExtractFolder; //if package type is extractable archive we need a extraction folder
    private string mExecutableName; //name of the executable
    private string mHelpText; //explaining why the tool is needed
}
'@

Function Get-ToolList{
	## Add class definition it to the powershell session
	Add-Type -TypeDefinition $toolType
	
	$toollist = @()

#Available tools
#--------------
	## New-Object Tool(
	#		Name, 
	#		DownloadURL, 
	#		SaveAs, 
	#		PackageType, 
	#		InstalledBinFolder,
	#       ExtraPaths
	#		ExtractFolder, 
	#		ExecutableName, 
	#		HelpText )

    # Microsoft Visual C++ Studio Community 2013
	$toollist += New-Object Tool(
				"MSVC2013", 
				"http://go.microsoft.com/?linkid=9863608", 
				"$ToolFolder\vs_community.exe", 
				"EXE", 
				"$script:CX_PROGRAM_FILES_X86\Microsoft Visual Studio 12.0\VC\bin", 
				"",
				"", 
				"nmake", 
				""
				)
    # 7-Zip 9.20 (x64)
    $toollist += New-Object Tool(
				"7-Zip", 
				"http://downloads.sourceforge.net/sevenzip/7z920-x64.msi", 
				"$ToolFolder\7-Zip-installer.msi", 
				"MSI", 
				"$script:CX_PROGRAM_FILES\7-Zip", 
				"",
				"", 
				"7z", 
				"Needed to untar CppUnit."
				)
    # CppUnit 1.12.1
    $toollist += New-Object Tool(
			   "cppunit", 
			   "http://sourceforge.net/projects/cppunit/files/cppunit/1.12.1/cppunit-1.12.1.tar.gz/download", 
			   "$ToolFolder\CppUnit.tar.gz", 
			   "TarGz", 
			   "$script:CX_EXTERNAL_CODE\cppunit-1.12.1", 
			   "$script:CX_EXTERNAL_CODE\cppunit-1.12.1\include",
			   "$script:CX_EXTERNAL_CODE", 
			   "", 
			   "Old way to write tests in CustusX."
			   )
    # git 1.7.10 (x86?)
    $toollist += New-Object Tool(
				"git", 
			    "https://github.com/msysgit/msysgit/releases/download/Git-1.9.4-preview20140929/Git-1.9.4-preview20140929.exe", 
			    "$ToolFolder\git-installer.exe", 
			    "Inno Setup package", 
			    "$script:CX_PROGRAM_FILES_X86\Git\cmd", 
				"",
			    "", 
			    "git", 
			    "Version control system."
			    )
	# ninja
    $toollist += New-Object Tool(
				"ninja", 
				"https://github.com/martine/ninja.git", 
				"", 
				"GitBootstrap", 
				"$script:CX_WORKSPACE\Ninja\Ninja",
				"",
				"", 
				"ninja", 
				"Enables support for compiling using more than one core.")
    # CMake 3.0.1 (x86)
    $toollist += New-Object Tool(
				"cmake", 
				"http://www.cmake.org/files/v3.0/cmake-3.0.1-win32-x86.exe", 
				"$ToolFolder\cmake-installer.exe", 
				"NSIS package", 
				"$script:CX_PROGRAM_FILES_X86\CMake\bin",
				"",
				"", 
				"cmake", 
				"For generating make files."
				)
    # NSIS 2.46
    $toollist += New-Object Tool(
				"nsis", 
				"http://downloads.sourceforge.net/project/nsis/NSIS%202/2.46/nsis-2.46-setup.exe?r=http%3A%2F%2Fnsis.sourceforge.net%2FDownload&ts=1346835031&use_mirror=garr", 
				"$ToolFolder\nsis-installer.exe", 
				"NSIS package", 
				"$script:CX_PROGRAM_FILES_X86\NSIS", 
				"", 
				"", 
				"NSIS", 
				"NSIS (Nullsoft Scriptable Install System) is a professional open source system to create Windows installers."
				)
    # CUDA 7.0.28 (x64, win8/win7)
    $toollist += New-Object Tool(
				"cuda",
				"http://developer.download.nvidia.com/compute/cuda/7_0/Prod/local_installers/cuda_7.0.28_windows.exe",
				"$ToolFolder\cuda_toolkit.exe", 
				"CUDAInstaller", 
				"$script:CX_PROGRAM_FILES\NVIDIA GPU Computing Toolkit\CUDA\v7.0\bin", 
				"$script:CX_PROGRAM_FILES\NVIDIA GPU Computing Toolkit\CUDA\v7.0\include",
				"", 
				"cudafe", 
				"The NVIDIA CUDA Toolkit provides a comprehensive development environment for C and C++ developers building GPU-accelerated applications."
				)
    # Python 2.7
    $toollist += New-Object Tool(
				"python", 
			    "http://www.python.org/ftp/python/2.7.3/python-2.7.3.msi", 
			    "$ToolFolder\python-installer.msi", 
			    "MSI", 
			    "$script:CX_DEFAULT_DRIVE\Python27",
				"",				
			    "", 
			    "python", 
			    "Needed to run cxInstaller.py script that will download, configure and build all needed code for CustusX."
			    )
    # Eclipse Luna (x86_64), java 1.7 required
    $toollist += New-Object Tool(
				"eclipse",
				"http://www.eclipse.org/downloads/download.php?file=/technology/epp/downloads/release/luna/SR2/eclipse-cpp-luna-SR2-win32-x86_64.zip&r=1",
			    "$ToolFolder\eclipse.zip", 
			    "ZIP", 
			    "$script:CX_PROGRAM_FILES\eclipse", 
				"",
			    "$script:CX_PROGRAM_FILES", 
			    "eclipse", 
			    "Editor. Kepler (4.3)"
				)
    # Qt 5.4 vs2013, 64 bit libs only, installer
    $toollist += New-Object Tool(
				"qt", 
				"http://download.qt.io/archive/qt/5.4/5.4.0/qt-opensource-windows-x86-msvc2013_64_opengl-5.4.0.exe", 
			    "$ToolFolder\qt.exe", 
			    "NSIS package", 
			    "$script:CX_DEFAULT_DRIVE\Qt\Qt5.4.0\5.4\msvc2013_64_opengl\bin",
				"$script:CX_DEFAULT_DRIVE\Qt\Qt5.4.0\Tools\QtCreator\bin",
			    "",
			    "qmake", 
			    "Only 64 bit libs."
			    )
    # Boost 1.56.0
    $toollist += New-Object Tool(
				"boost", 
				"http://downloads.sourceforge.net/project/boost/boost-binaries/1.56.0/boost_1_56_0-msvc-12.0-64.exe?r=http%3A%2F%2Fsourceforge.net%2Fprojects%2Fboost%2Ffiles%2Fboost-binaries%2F1.56.0%2F&ts=1410340910&use_mirror=softlayer-ams", 
				"$ToolFolder\boost.exe", 
				"Inno Setup package", 
				"$script:CX_DEFAULT_DRIVE\local\boost_1_56_0", 
				"$script:CX_DEFAULT_DRIVE\local\boost_1_56_0\lib64-msvc-12.0",
				"", 
				"", 
				"Utility library."
				)
    # Console2 2.00b148Beta (x86)
    $toollist += New-Object Tool(
				"console2", 
				"http://downloads.sourceforge.net/project/console/console-devel/2.00/Console-2.00b148-Beta_32bit.zip?r=http%3A%2F%2Fsourceforge.net%2Fprojects%2Fconsole%2F&ts=1345702855&use_mirror=garr", 
				"$ToolFolder\console2.zip", 
				"ZIP", 
				"$script:CX_PROGRAM_FILES_X86\Console2", 
				"",
				"$script:CX_PROGRAM_FILES_X86", 
				"Console", 
				"Console is a Windows console window enhancement."
				)
	<# Not needed
    # ITK-SNAP 2.2.0 (x64)
    $toollist += New-Object Tool(
				"ITK-Snap", 
			    "http://sourceforge.net/projects/itk-snap/files/itk-snap/2.2.0//itksnap-2.2.0-20110504-win64-x64.exe/download", 
			    "$ToolFolder\ITKSnap.exe",
			    "NSIS package", 
			    "$script:CX_PROGRAM_FILES_X86\ITK-SNAP 2.2\bin", 
				"", 
			    "", 
			    "InsightSNAP.exe", 
			    "Software application used to segment structures in 3D medical images."
				)
	#>
    # Universal Silent Switch Finder 1.5.0.0
    $toollist += New-Object Tool(
				"ussf", 
				"http://download.softpedia.com/dl/2f06f2807bcc03275a125f4f0e7eae3e/5589272f/100180984/software/system/ussf.exe",
				"$ToolFolder\ussf.exe", 
				"", 
				"$ToolFolder", 
				"",
				"", 
				"ussf", 
				"Useful tool for detecting installer switches."
				)
							
	return $toollist
}