; ================= Fraxinus post-install =================

StrCpy $9 "$LOCALAPPDATA\\Fraxinus\\Logs"
CreateDirectory "$9"

SectionGetFlags ${RaidionicsSetup} $0
IntOp $0 $0 & ${SF_SELECTED}
IntCmp $0 0 +5
  DetailPrint "Running PowerShell: installRaidionics.ps1"
  ExecWait '"$WINDIR/System32/WindowsPowerShell/v1.0/powershell.exe" -ExecutionPolicy Bypass -NoLogo -NonInteractive -File "$INSTDIR/installRaidionics.ps1"' $1
  DetailPrint "  -> Raidionics exit code: $1"

SectionGetFlags ${TotalSegSetup} $2
IntOp $2 $2 & ${SF_SELECTED}
IntCmp $2 0 +5
  DetailPrint "Running PowerShell: installTotalsegmentator.ps1"
  ExecWait '"$WINDIR/System32/WindowsPowerShell/v1.0/powershell.exe" -ExecutionPolicy Bypass -NoLogo -NonInteractive -File "$INSTDIR/installTotalsegmentator.ps1"' $3
  DetailPrint "  -> TotalSeg exit code: $3"

SectionGetFlags ${ElastixSetup} $4
IntOp $4 $4 & ${SF_SELECTED}
IntCmp $4 0 +5
  DetailPrint "Running PowerShell: installElastix.ps1"
  ExecWait '"$WINDIR/System32/WindowsPowerShell/v1.0/powershell.exe" -ExecutionPolicy Bypass -NoLogo -NonInteractive -File "$INSTDIR/installElastix.ps1"' $5
  DetailPrint "  -> TotalSeg exit code: $5"


; ================= End Fraxinus post-install =================