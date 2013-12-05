REM Experimental
REM http://stackoverflow.com/questions/357315/get-list-of-passed-arguments-in-windows-batch-script-bat

set TARGET_PLATFORM=%1
set CX_ENVSCRIPTS=%2
set COMMAND=%3
set SCRIPT_PATH=%~dp0
echo "hit the win batch script!"

echo "args:" %*

if "%TARGET_PLATFORM%" == "win64" (
  set INIT_VS=%CX_ENVSCRIPTS%\cxVars_x64.bat
  set ARCH_FLAG="--b32"
)

if "%TARGET_PLATFORM%" == "win32" (
  set INIT_VS=%CX_ENVSCRIPTS%\cxVars_x86.bat
)

REM %INIT_VS% && python -u %SCRIPT_PATH%\cxJenkinsBuildStep.py
REM echo "NO1 win batch script complete!"

%INIT_VS% && python -u %SCRIPT_PATH%\cxJenkinsBuildStep.py --root_dir D:\j\ws\unit\TARGET\win64
echo "NO1 win batch script complete!"

%INIT_VS% && python -u %SCRIPT_PATH%\%COMMAND% %ARCH_FLAG%
echo "NO2 win batch script complete!"

echo "win batch script complete!"

