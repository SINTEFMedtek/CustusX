REM Experimental

set TARGET_PLATFORM=%1
set CX_ENVSCRIPTS=%2
set COMMAND=%3
echo "hit the win batch script!"

echo "args:" %*

if "%TARGET_PLATFORM%" == "win64" (
  set INIT_VS=%CX_ENVSCRIPTS%\cxVars_x64.bat
  set ARCH_FLAG="--b32"
)

if "%TARGET_PLATFORM%" == "win32" (
  set INIT_VS=%CX_ENVSCRIPTS%\cxVars_x86.bat
)

%INIT_VS% && python -u cxJenkinsBuildStep.py
echo "NO1 win batch script complete!"

%INIT_VS% && python -u cxJenkinsBuildStep.py --root_dir D:\j\ws\unit\TARGET\win64
echo "NO1 win batch script complete!"

%INIT_VS% && python -u %COMMAND% %ARCH_FLAG%
echo "NO2 win batch script complete!"

echo "win batch script complete!"

