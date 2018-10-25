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

    # Microsoft Visual C++ Studio Community 2015
	$toollist += New-Object Tool(
				"MSVC2015", 
				"http://go.microsoft.com/fwlink/?LinkId=691978&clcid=0x409", 
				"$ToolFolder\vs_community_ENU.exe", 
				"EXE", 
				"$script:CX_PROGRAM_FILES_X86\Microsoft Visual Studio 14.0\VC\bin", 
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
    # GLEW 1.13.0
    $toollist += New-Object Tool(
			   "glew", 
			   "http://downloads.sourceforge.net/project/glew/glew/1.13.0/glew-1.13.0-win32.zip?r=http%3A%2F%2Fglew.sourceforge.net%2Findex.html&ts=1456483577&use_mirror=netcologne", 
			   "$ToolFolder\GLEW.zip", 
			   "ZIP", 
			   "$script:CX_EXTERNAL_CODE\glew-1.13.0\bin\Release\x64", 
			   "$script:CX_EXTERNAL_CODE\glew-1.13.0\lib\Release\x64;$script:CX_EXTERNAL_CODE\glew-1.13.0\include;$script:CX_EXTERNAL_CODE\glew-1.13.0\include\GL",
			   "$script:CX_EXTERNAL_CODE", 
			   "glewinfo.exe", 
			   "Library needed by FAST."
			   )
    # git 2.13.0
    $toollist += New-Object Tool(
				"git", 
			    "https://npm.taobao.org/mirrors/git-for-windows/2.13.0.windows.1/Git-2.13.0-64-bit.exe",
			    "$ToolFolder\git-installer.exe", 
			    "Inno Setup package", 
			    "$script:CX_PROGRAM_FILES\Git\bin",
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
				"Enables support for compiling using more than one core."
                )
    # CMake 3.8.1 (x86)
    $toollist += New-Object Tool(
				"cmake", 
				"https://cmake.org/files/v3.8/cmake-3.8.1-win32-x86.msi", 
				"$ToolFolder\cmake-installer.msi", 
				"MSI", 
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
    # CUDA 8.0.61 (x64, win10)
    $toollist += New-Object Tool(
				"cuda",
				"https://developer.nvidia.com/compute/cuda/8.0/Prod2/local_installers/cuda_8.0.61_win10-exe",
				"$ToolFolder\cuda_toolkit.exe", 
				"CUDAInstaller", 
				"$script:CX_PROGRAM_FILES\NVIDIA GPU Computing Toolkit\CUDA\v8.0\bin",
				"$script:CX_PROGRAM_FILES\NVIDIA GPU Computing Toolkit\CUDA\v8.0\include",
				"", 
				"cudafe", 
				"The NVIDIA CUDA Toolkit provides a comprehensive development environment for C and C++ developers building GPU-accelerated applications."
				)
    # Python 2.7
    $toollist += New-Object Tool(
				"python", 
				"https://www.python.org/ftp/python/2.7.11/python-2.7.11.amd64.msi", 
			    "$ToolFolder\python-installer.msi", 
			    "MSI", 
			    "$script:CX_DEFAULT_DRIVE\Python27;$script:CX_DEFAULT_DRIVE\Python27\Scripts",
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
    # Qt 5.9.6, select mvs 2015, 64 bit
    $toollist += New-Object Tool(
				"qt", 
                "http://download.qt.io/archive/qt/5.9/5.9.6/qt-opensource-windows-x86-5.9.6.exe",
			    "$ToolFolder\qt.exe", 
			    "NSIS package", 
			    "$script:CX_DEFAULT_DRIVE\Qt\Qt5.6.0\5.6\msvc2015_64\bin",
				"$script:CX_DEFAULT_DRIVE\Qt\Qt5.6.0\Tools\QtCreator\bin",
			    "",
			    "qmake", 
			    "Only 64 bit libs."
			    )
    # Boost 1.60.0
    $toollist += New-Object Tool(
				"boost", 
				"https://sourceforge.net/projects/boost/files/boost-binaries/1.60.0/boost_1_60_0-msvc-14.0-64.exe/download", 
				"$ToolFolder\boost.exe", 
				"Inno Setup package", 
				"$script:CX_DEFAULT_DRIVE\local\boost_1_60_0", 
				"$script:CX_DEFAULT_DRIVE\local\boost_1_60_0\lib64-msvc-14.0",
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
				"http://www.softpedia.com/dyn-postdownload.php/a5af3d7a8fa43dc5e12712c1051fc259/572360c1/2c2f8/4/1?tsf=0",
				"$ToolFolder\ussf.exe", 
				"", 
				"$ToolFolder", 
				"",
				"", 
				"ussf", 
				"Useful tool for detecting installer switches."
				)
	# Doxygen
    $toollist += New-Object Tool(
				"doxygen", 
				"", 
				"", 
				"", 
				"", 
				"C:\Program Files\doxygen\bin", 
				"", 
				"", 
				"For creating documentation."
				)
							
	return $toollist
}