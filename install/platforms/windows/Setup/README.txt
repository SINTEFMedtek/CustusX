DISCLAIMER:
This script package is developed to be used as a fresh install.
Using it to update your system might or might not work. :)

Prerequisites:
- GitHub account with uploaded id_rsa.pub (public ssh-key)- Windows 10, 64 bit
- Manual installation of Visual studio 2015 community edition. See problem 0 below.
- Powershell installed
- NVIDIA graphicscard - because of cuda

Caveat:
- Only supports 64 bit compilation

---------------------------------------------------------------------------------------------------------------------------
SETUP:
---------------------------------------------------------------------------------------------------------------------------
1) Make sure Windows has installed ALL updates!
2) Edit Config.ps1 (if needed)
3) Start -> Search for cmd -> Right click -> Run as Administrator
4) C:\User\dev\Desktop\Setup\> Setup.bat
4a) You will need to confirm that the installation should continue after reading what it will do, this will happen twice
NOTE) When the installer tries to install Qt you have to manually click install for the script to continue
5) Wait for the script to finish
---------------------------------------------------------------------------------------------------------------------------

PROBLEM SOLVING:

(Problem 0)
Visual studio 2015 community edition must be installed manually. At the time no working direct 
link to the installer which the script could use was found. Also, remember to choose
custom installation and make sure to install the C++ compiler.

(Problem 1) 
After installing eclipse I get the following error:
"A Java Runtime Environment (JRE) or Java Development Kit (JDK)
must be available in order to run Eclipse. No Java virtual machine
was found after searching the following locations:
..."
(Solution 1)
Just install either the latest JRE (or JDK).

(Problem 2)
Could not use Qt 4.8.1 that comes with an installer and that is precompiled. 
(Solution 2)
The source was then downloaded to your computer, you now need to build it
yourself, as 32 bit and/or 64 bit. 
THIS IS A INSOURCE BUILD, this means you need a complete source folder for 
each type of build (32/64bit) you want to do.
Do this:
1.	Use Visual Studio Command Prompt (32bit) or Visual Studio 2005 x64 Win64 Command Prompt (64bit).
2.	Configuring will take 30ish minutes to finish:
    1.	Cd to source folder
    2.	> configure -mp -debug-and-release -opensource
        1.	-mp = multi processor build
        2.	–debug-and-release = builds both release and debug libraries.
    3.	Accept license, "y + enter"
3.	> nmake 
    1.	Can take many hours to finish.
    
(Problem 3)
Anti-virus finds potentially harmfull program, something about the compiler.
(Solution3)
Tell anti-virus to ignore.

(Problem 4)
When configuring ITK the following error occurs:
"CMake Error at CMakeLists.txt:22 (message):
  ITK build directory path length is too long (51 > 50).Please set the ITK
  build directory to a directory with a shorter path."
http://www.mail-archive.com/insight-developers@itk.org/msg00181.html
(Solution4)
Get CMake 2.8.9 or higher.

(Problem 5)
Howto: Install python on Windows (for CustusX)

*) Use 64bit Python 2.7.X - download from python.org
*) Install setuptools
*) Install pip

*) Install lxml (xml manipulation)
http://www.lfd.uci.edu/~gohlke/pythonlibs/2it9ckwi/lxml-3.2.3.win-amd64-py2.7.exe

*) Install paramiko, needed for ssh/scp
http://www.voidspace.org.uk/downloads/pycrypto26/pycrypto-2.6.win-amd64-py2.7.exe
pip install ecdsa
pip install paramiko

(Problem 6)
Script stops after installing Ninja.
(Solution)
Break script and start again.
