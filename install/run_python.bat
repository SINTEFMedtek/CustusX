:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Jenkins control script
::   Author: Christian Askeland, SINTEF Medical Technology
::   Date:   2013.12.05
:: Run this script as a bridge form Jenkins XShell plugin to python calls in the Windows case.
:: Usage: path/to/run_python <target_os> <cx_envscripts_path> "<python_script> <arguments>"
::   Note the hyphens around the third argument containing script name + args.
:: 
:: http://stackoverflow.com/questions/357315/get-list-of-passed-arguments-in-windows-batch-script-bat

echo "Batch arguments: " %*

set TARGET_PLATFORM=%1
set CX_ENVSCRIPTS=%2
set COMMAND=%~3
set SCRIPT_PATH=%~dp0

if "%TARGET_PLATFORM%" == "win64" (
    set INIT_VS=%CX_ENVSCRIPTS%\cxVars_x64.bat
)
if "%TARGET_PLATFORM%" == "win32" (
    set INIT_VS=%CX_ENVSCRIPTS%\cxVars_x86.bat
    set ARCH_FLAG=--b32
)

%INIT_VS% && python -u %SCRIPT_PATH%\%COMMAND% %ARCH_FLAG%


