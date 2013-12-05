REM Experimental

TARGET_PLATFORM=%1
CX_ENVSCRIPTS=%2
COMMAND=%3
echo "hit the win batch script!"

echo "args:" %*

if "%TARGET_PLATFORM%" == "win64" 
(
  INIT_VS=%CX_ENVSCRIPTS%\cxVars_x64.bat
  ARCH_FLAG="--b32"
)

if "%TARGET_PLATFORM%" == "win32" 
(
  INIT_VS=%CX_ENVSCRIPTS%\cxVars_x86.bat
)

%INIT_VS% && python -u %COMMAND% %ARCH_FLAG%

