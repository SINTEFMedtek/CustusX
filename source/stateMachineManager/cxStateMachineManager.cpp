#include "cxStateMachineManager.h"

#include <iostream>
#include <QByteArray>
#include <QSettings>
#include <QDir>
#include "sscXmlOptionItem.h"
#include "sscMessageManager.h"
#include "cxDataLocations.h"
#include "cxPatientData.h"
#include "cxWorkflowStateMachine.h"
#include "cxApplicationStateMachine.h"

namespace cx
{
Desktop::Desktop()
{}

Desktop::Desktop(QString layout, QByteArray mainwindowstate) :
    mLayoutUid(layout),
    mMainWindowState(mainwindowstate)
{}

class ApplicationsParser
{
public:
  ApplicationsParser()
  {
    mXmlFile = ssc::XmlOptionFile(DataLocations::getXmlSettingsFile(),"CustusX").descend("applications");

    //Lab application
    this->addDefaultDesktops("PatientDataUid", "LAYOUT_3DACS_1X3", "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");
    this->addDefaultDesktops("NavigationUid", "LAYOUT_3DACS_1X3", "AAAA/wAAAAD9AAAAAgAAAAAAAAGBAAAEOPwCAAAAF/sAAAAUAEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAALAAAArQAAAJCAP////sAAAAiAEMAbwBuAHQAZQB4AHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAsAAABLAAAAAAAAAAA+wAAADIASQBtAGEAZwBlAFAAcgBvAHAAZQByAHQAaQBlAHMARABvAGMAawBXAGkAZABnAGUAdAEAAAM0AAAAuwAAAAAAAAAA+wAAABQARABvAGMAawBXAGkAZABnAGUAdAAAAAAsAAABvwAAAAAAAAAA+wAAADwATQBlAHMAaABQAHIAbwBwAGUAcgB0AGkAZQBzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAALAAABDgAAAEUAP////sAAABGAFIAZQBnAGkAcwB0AHIAYQB0AGkAbwBuAEgAaQBzAHQAbwByAHkAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAEwA////+wAAAC4AQgByAG8AdwBzAGUAcgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAApgD////7AAAAQABUAHIAYQBuAHMAZgBlAHIARgB1AG4AYwB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAAAAAAAAPsAAAA+AFMAaABpAGYAdABDAG8AcgByAGUAYwB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAAAAAAAAPsAAABCAEkAbQBhAGcAZQBSAGUAZwBpAHMAdAByAGEAdABpAG8AbgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAAAAAAAAD7AAAARgBQAGEAdABpAGUAbgB0AFIAZQBnAGkAcwB0AHIAYQB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAAAAAAAAPsAAAA0AE4AYQB2AGkAZwBhAHQAaQBvAG4AVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAOsA////+wAAAEAAVgBvAGwAdQBtAGUAUAByAG8AcABlAHIAdABpAGUAcwBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAABZwD////7AAAALgBDAG8AbgBzAG8AbABlAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAABmAP////sAAAAuAEkARwBUAEwAaQBuAGsAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAANwA////+wAAAEIAVAByAGEAYwBrAGUAZABDAGUAbgB0AGUAcgBsAGkAbgBlAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAACsAP////sAAAA4AFUAUwBBAGMAcQB1AHMAaQB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAADwAP////sAAAA6AEMAYQBtAGUAcgBhAEMAbwBuAHQAcgBvAGwAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAP0A////+wAAAEYAUgBlAGcAaQBzAHQAcgBhAHQAaQBvAG4ATQBlAHQAaABvAGQAcwBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAACZAD////7AAAARgBTAGUAZwBtAGUAbgB0AGEAdABpAG8AbgBNAGUAdABoAG8AZABzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAAgAP////sAAABIAFYAaQBzAHUAYQBsAGkAegBhAHQAaQBvAG4ATQBlAHQAaABvAGQAcwBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAAkgD////7AAAARABDAGEAbABpAGIAcgBhAHQAaQBvAG4ATQBlAHQAaABvAGQAcwBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAACmQD////7AAAAMgBGAHIAYQBtAGUAVAByAGUAZQBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAAggD///8AAAACAAAGkAAAALD8AQAAAAP7AAAAPABUAG8AbwBsAFAAcgBvAHAAZQByAHQAaQBlAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAAAAABdgAAAWcA////+wAAADoAUABvAGkAbgB0AFMAYQBtAHAAbABpAG4AZwBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAAAAALbAAABJgD////7AAAAPgBJAG0AYQBnAGUAUAByAG8AcABlAHIAdABpAGUAcwBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAAAAAaQAAABBAD///8AAAaQAAADwwAAAAQAAAAEAAAACAAAAAj8AAAAAgAAAAAAAAAAAAAAAgAAAAcAAAAWAEQAYQB0AGEAVABvAG8AbABCAGEAcgAAAAAA/////wAAAAAAAAAAAAAAHgBXAG8AcgBrAGYAbABvAHcAVABvAG8AbABCAGEAcgEAAAAA/////wAAAAAAAAAAAAAAIgBOAGEAdgBpAGcAYQB0AGkAbwBuAFQAbwBvAGwAQgBhAHIBAAABzP////8AAAAAAAAAAAAAACYAQwBhAG0AZQByAGEAMwBEAFYAaQBlAHcAVABvAG8AbABCAGEAcgAAAAOs/////wJHFoMAI4CEAAAAFgBUAG8AbwBsAFQAbwBvAGwAQgBhAHIBAAADrP////8AAAAAAAAAAAAAABwARABlAHMAawB0AG8AcABUAG8AbwBsAEIAYQByAQAABBb/////AAAAAAAAAAAAAAAWAEgAZQBsAHAAVABvAG8AbABCAGEAcgEAAAR7/////wAAAAAAAAAA");
    this->addDefaultDesktops("RegistrationUid", "LAYOUT_3DACS_1X3", "AAAA/wAAAAD9AAAABAAAAAAAAAGEAAADw/wCAAAAFfsAAABGAFIAZQBnAGkAcwB0AHIAYQB0AGkAbwBuAE0AZQB0AGgAbwBkAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAEAAAAsAAADwwAAAmQA////+wAAABQARABvAGMAawBXAGkAZABnAGUAdAAAAAAsAAACtAAAAkIA////+wAAACIAQwBvAG4AdABlAHgAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAACwAAAEsAAAAAAAAAAD7AAAAMgBJAG0AYQBnAGUAUAByAG8AcABlAHIAdABpAGUAcwBEAG8AYwBrAFcAaQBkAGcAZQB0AQAAAzQAAAC7AAAAAAAAAAD7AAAAFABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAACwAAAG/AAAAAAAAAAD7AAAAPABNAGUAcwBoAFAAcgBvAHAAZQByAHQAaQBlAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAsAAAEOAAAARQA////+wAAAEYAUgBlAGcAaQBzAHQAcgBhAHQAaQBvAG4ASABpAHMAdABvAHIAeQBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAATAD////7AAAALgBCAHIAbwB3AHMAZQByAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAACmAP////sAAABAAFQAcgBhAG4AcwBmAGUAcgBGAHUAbgBjAHQAaQBvAG4AVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAAAAAAAA+wAAAD4AUwBoAGkAZgB0AEMAbwByAHIAZQBjAHQAaQBvAG4AVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAAAAAAAA+wAAAEIASQBtAGEAZwBlAFIAZQBnAGkAcwB0AHIAYQB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAAAAAAAAPsAAABGAFAAYQB0AGkAZQBuAHQAUgBlAGcAaQBzAHQAcgBhAHQAaQBvAG4AVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAAAAAAAA+wAAADQATgBhAHYAaQBnAGEAdABpAG8AbgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAA6wD////7AAAAMgBGAHIAYQBtAGUAVAByAGUAZQBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAAggD////7AAAALgBJAEcAVABMAGkAbgBrAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAADcAP////sAAABCAFQAcgBhAGMAawBlAGQAQwBlAG4AdABlAHIAbABpAG4AZQBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAArAD////7AAAAOABVAFMAQQBjAHEAdQBzAGkAdABpAG8AbgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAA8AD////7AAAAOgBDAGEAbQBlAHIAYQBDAG8AbgB0AHIAbwBsAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAD9AP////sAAABGAFMAZQBnAG0AZQBuAHQAYQB0AGkAbwBuAE0AZQB0AGgAbwBkAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAACAA////+wAAAEgAVgBpAHMAdQBhAGwAaQB6AGEAdABpAG8AbgBNAGUAdABoAG8AZABzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAACSAP////sAAABEAEMAYQBsAGkAYgByAGEAdABpAG8AbgBNAGUAdABoAG8AZABzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAKZAP///wAAAAEAAAGEAAADw/wCAAAAAfsAAABAAFYAbwBsAHUAbQBlAFAAcgBvAHAAZQByAHQAaQBlAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAsAAADwwAAAWcA////AAAAAgAABpAAAACw/AEAAAAD+wAAADwAVABvAG8AbABQAHIAbwBwAGUAcgB0AGkAZQBzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAAAAAXYAAAFnAP////sAAAA6AFAAbwBpAG4AdABTAGEAbQBwAGwAaQBuAGcAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAAAAAC2wAAASYA////+wAAAD4ASQBtAGEAZwBlAFAAcgBvAHAAZQByAHQAaQBlAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAAAAAGkAAAAQQA////AAAAAwAAB4AAAADo/AEAAAAB+wAAAC4AQwBvAG4AcwBvAGwAZQBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAAAAAeAAAAATgD///8AAAULAAADwwAAAAQAAAAEAAAACAAAAAj8AAAAAQAAAAIAAAAHAAAAFgBEAGEAdABhAFQAbwBvAGwAQgBhAHIAAAAAAP////8AAAAAAAAAAAAAACIATgBhAHYAaQBnAGEAdABpAG8AbgBUAG8AbwBsAEIAYQByAAAAAAD/////AAAAAAAAAAAAAAAeAFcAbwByAGsAZgBsAG8AdwBUAG8AbwBsAEIAYQByAQAAAAAAAAHVAAAAAAAAAAAAAAAmAEMAYQBtAGUAcgBhADMARABWAGkAZQB3AFQAbwBvAGwAQgBhAHIBAAAB1f////8AAAAAAAAAAAAAABYAVABvAG8AbABUAG8AbwBsAEIAYQByAQAAAvj/////AAAAAAAAAAAAAAAcAEQAZQBzAGsAdABvAHAAVABvAG8AbABCAGEAcgEAAANiAAAAbAAAAAAAAAAAAAAAFgBIAGUAbABwAFQAbwBvAGwAQgBhAHIBAAADzgAAAqcAAAAAAAAAAA==");
    this->addDefaultDesktops("PreOpPlanningUid", "LAYOUT_3DACS_1X3", "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");
    this->addDefaultDesktops("IntraOpImagingUid", "LAYOUT_US_Acquisition", "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");
    this->addDefaultDesktops("PostOpControllUid", "LAYOUT_3DACS_1X3", "AAAA/wAAAAD9AAAAAwAAAAAAAAGBAAADw/wCAAAAFvsAAAAUAEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAALAAAAqYAAAJCAP////sAAAAiAEMAbwBuAHQAZQB4AHQARABvAGMAawBXAGkAZABnAGUAdAEAAAMgAAAAWgAAAAAAAAAA+wAAADIASQBtAGEAZwBlAFAAcgBvAHAAZQByAHQAaQBlAHMARABvAGMAawBXAGkAZABnAGUAdAEAAAM0AAAAuwAAAAAAAAAA+wAAABQARABvAGMAawBXAGkAZABnAGUAdAEAAAN7AAAAYAAAAAAAAAAA+wAAADwATQBlAHMAaABQAHIAbwBwAGUAcgB0AGkAZQBzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAALAAAAW0AAAEUAP////sAAABGAFIAZQBnAGkAcwB0AHIAYQB0AGkAbwBuAEgAaQBzAHQAbwByAHkAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAEwA////+wAAAC4AQgByAG8AdwBzAGUAcgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AQAAACwAAAPDAAAApgD////7AAAAQABUAHIAYQBuAHMAZgBlAHIARgB1AG4AYwB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAAAAAAAAPsAAAA+AFMAaABpAGYAdABDAG8AcgByAGUAYwB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAAAAAAAAPsAAABCAEkAbQBhAGcAZQBSAGUAZwBpAHMAdAByAGEAdABpAG8AbgBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAACwAAAS0AAAAAAAAAAD7AAAARgBQAGEAdABpAGUAbgB0AFIAZQBnAGkAcwB0AHIAYQB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAAAAAAAAPsAAAA0AE4AYQB2AGkAZwBhAHQAaQBvAG4AVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAOsA////+wAAAEAAVgBvAGwAdQBtAGUAUAByAG8AcABlAHIAdABpAGUAcwBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAABZwD////7AAAARgBSAGUAZwBpAHMAdAByAGEAdABpAG8AbgBNAGUAdABoAG8AZABzAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAAJkAP////sAAAAuAEkARwBUAEwAaQBuAGsAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAANwA////+wAAAEIAVAByAGEAYwBrAGUAZABDAGUAbgB0AGUAcgBsAGkAbgBlAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAACsAP////sAAAA4AFUAUwBBAGMAcQB1AHMAaQB0AGkAbwBuAFcAaQBkAGcAZQB0AEQAbwBjAGsAVwBpAGQAZwBlAHQAAAAAAP////8AAADwAP////sAAAA6AEMAYQBtAGUAcgBhAEMAbwBuAHQAcgBvAGwAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAP0A////+wAAAEYAUwBlAGcAbQBlAG4AdABhAHQAaQBvAG4ATQBlAHQAaABvAGQAcwBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAAIAD////7AAAASABWAGkAcwB1AGEAbABpAHoAYQB0AGkAbwBuAE0AZQB0AGgAbwBkAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAJIA////+wAAAEQAQwBhAGwAaQBiAHIAYQB0AGkAbwBuAE0AZQB0AGgAbwBkAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAApkA////+wAAADIARgByAGEAbQBlAFQAcgBlAGUAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAA/////wAAAIIA////AAAAAQAAAAAAAAAA/AIAAAAB+wAAAC4AQwBvAG4AcwBvAGwAZQBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAD/////AAAAZgD///8AAAACAAAGkAAAALD8AQAAAAP7AAAAPABUAG8AbwBsAFAAcgBvAHAAZQByAHQAaQBlAHMAVwBpAGQAZwBlAHQARABvAGMAawBXAGkAZABnAGUAdAAAAAAAAAABdgAAAWcA////+wAAADoAUABvAGkAbgB0AFMAYQBtAHAAbABpAG4AZwBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAAAAALbAAABJgD////7AAAAPgBJAG0AYQBnAGUAUAByAG8AcABlAHIAdABpAGUAcwBXAGkAZABnAGUAdABEAG8AYwBrAFcAaQBkAGcAZQB0AAAAAAAAAAaQAAABBAD///8AAAUOAAADwwAAAAQAAAAEAAAACAAAAAj8AAAAAQAAAAIAAAAHAAAAFgBEAGEAdABhAFQAbwBvAGwAQgBhAHIAAAAAAP////8AAAAAAAAAAAAAABYAVABvAG8AbABUAG8AbwBsAEIAYQByAAAAAAD/////AAAAAAAAAAAAAAAiAE4AYQB2AGkAZwBhAHQAaQBvAG4AVABvAG8AbABCAGEAcgAAAAAA/////wAAAAAAAAAAAAAAHgBXAG8AcgBrAGYAbABvAHcAVABvAG8AbABCAGEAcgEAAAAA/////wAAAAAAAAAAAAAAJgBDAGEAbQBlAHIAYQAzAEQAVgBpAGUAdwBUAG8AbwBsAEIAYQByAQAAAcz/////AAAAAAAAAAAAAAAcAEQAZQBzAGsAdABvAHAAVABvAG8AbABCAGEAcgEAAALv/////wAAAAAAAAAAAAAAFgBIAGUAbABwAFQAbwBvAGwAQgBhAHIBAAADVP////8AAAAAAAAAAA==");
    this->addDefaultDesktops("DEFAULT", "LAYOUT_3DACS_1X3", "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");
  };

  ~ApplicationsParser(){};

  void addDefaultDesktops(QString workflowStateUid, QString layoutUid, QString mainwindowstate)
  {
    mWorkflowDefaultDesktops[workflowStateUid] = Desktop(layoutUid,QByteArray::fromBase64(mainwindowstate.toAscii()));
  }

  Desktop getDefaultDesktop(QString applicationName, QString workflowName)
  {
    //TODO use applicationName!!!
    if(!mWorkflowDefaultDesktops.count(workflowName))
      return mWorkflowDefaultDesktops["DEFAULT"];
    return mWorkflowDefaultDesktops[workflowName];
  }

  Desktop getDesktop(QString applicationName, QString workflowName)
  {
    Desktop retval;
    QDomElement workflowElement = mXmlFile.descend(applicationName).descend("workflows").descend(workflowName).getElement();
    QDomElement desktopElement;
    if(workflowElement.namedItem("custom").isNull())
    {
      //desktopElement = workflowElement.namedItem("default").toElement();
      return this->getDefaultDesktop(applicationName, workflowName);
    }
    else
      desktopElement = workflowElement.namedItem("custom").toElement();
    retval.mMainWindowState = QByteArray::fromBase64(desktopElement.attribute("mainwindowstate").toAscii());
    retval.mLayoutUid = desktopElement.attribute("layoutuid");

    return retval;
  };

  void setDesktop(QString applicationName, QString workflowName, Desktop desktop)
  {
    QDomElement desktopElement = mXmlFile.descend(applicationName).descend("workflows").descend(workflowName).descend("custom").getElement();
    desktopElement.setAttribute("mainwindowstate", QString(desktop.mMainWindowState.toBase64()));
    desktopElement.setAttribute("layoutuid", desktop.mLayoutUid);
    mXmlFile.save();
  };

  void resetDesktop(QString applicationName, QString workflowName)
  {
    QDomElement workflowElement = mXmlFile.descend(applicationName).descend("workflows").descend(workflowName).getElement();
    workflowElement.removeChild(workflowElement.namedItem("custom"));
    mXmlFile.save();
  };

private:
  ssc::XmlOptionFile mXmlFile;
  std::map<QString, Desktop> mWorkflowDefaultDesktops;
};

/// -------------------------------------------------------
/// -------------------------------------------------------
/// -------------------------------------------------------

StateManager *StateManager::mTheInstance = NULL;
StateManager* stateManager() { return StateManager::getInstance(); }
StateManager* StateManager::getInstance()
{
  if(mTheInstance == NULL)
   {
     mTheInstance = new StateManager();
     mTheInstance->initialize();
   }
   return mTheInstance;
}

void StateManager::destroyInstance()
{
    delete mTheInstance;
    mTheInstance = NULL;
}

StateManager::StateManager()
{

}

StateManager::~StateManager()
{}

WorkflowStateMachinePtr StateManager::getWorkflow()
{
  return mWorkflowStateMachine;
}
ApplicationStateMachinePtr StateManager::getApplication()
{
  return mApplicationStateMachine;
}

void StateManager::initialize()
{
  QSettingsPtr settings = DataLocations::getSettings();
  // Initialize settings if empty
  if (!settings->contains("globalPatientDataFolder"))
    settings->setValue("globalPatientDataFolder", QDir::homePath()+"/Patients");
  if (!settings->contains("globalApplicationName"))
    settings->setValue("globalApplicationName", "Lab");
  if (!settings->contains("globalPatientNumber"))
    settings->setValue("globalPatientNumber", 1);
  //if (!mSettings->contains("applicationNames"))
  //settings->setValue("applicationNames", "Nevro,Lap,Vasc,Lung,Lab");

  mPatientData.reset(new PatientData());

  mApplicationStateMachine.reset(new ApplicationStateMachine());
  mApplicationStateMachine->start();

  mWorkflowStateMachine.reset(new WorkflowStateMachine());
  mWorkflowStateMachine->start();
}

PatientDataPtr StateManager::getPatientData()
{
  return mPatientData;
}

Desktop StateManager::getActiveDesktop()
{
  ApplicationsParser parser;
  //std::cout << "Getting desktop for state: " << mWorkflowStateMachine->getActiveUidState().toStdString() << std::endl;
  return parser.getDesktop(mApplicationStateMachine->getActiveUidState(), mWorkflowStateMachine->getActiveUidState());
}

void StateManager::saveDesktop(Desktop desktop)
{
  ApplicationsParser parser;
  parser.setDesktop(mApplicationStateMachine->getActiveUidState(), mWorkflowStateMachine->getActiveUidState(), desktop);
}

void StateManager::resetDesktop()
{
  ApplicationsParser parser;
  parser.resetDesktop(mApplicationStateMachine->getActiveUidState(), mWorkflowStateMachine->getActiveUidState());
}

void StateManager::addRecordSession(RecordSessionPtr session)
{
  ssc::messageManager()->sendDebug("Added session with description "+session->getDescription());
  mRecordSessions.push_back(session);
  emit recordedSessionsChanged();
}

void StateManager::removeRecordSession(RecordSessionPtr session)
{
  std::vector<RecordSessionPtr>::iterator it = mRecordSessions.begin();
  for(; it != mRecordSessions.end(); ++it)
  {
    if((*it)->getUid() == session->getUid())
      mRecordSessions.erase(it);
  }
  emit recordedSessionsChanged();
}

std::vector<RecordSessionPtr> StateManager::getRecordSessions()
{
  return mRecordSessions;
}

RecordSessionPtr StateManager::getRecordSession(QString uid)
{
  RecordSessionPtr retval;
  std::vector<RecordSessionPtr>::iterator it = mRecordSessions.begin();
  for(; it != mRecordSessions.end(); ++it)
  {
    if((*it)->getUid() == uid)
      retval = (*it);
  }
  return retval;
}

void StateManager::addXml(QDomNode& parentNode)
{
  QDomDocument doc = parentNode.ownerDocument();
  QDomElement base = doc.createElement("stateManager");
  parentNode.appendChild(base);

  QDomElement sessionsNode = doc.createElement("recordSessions");
  std::vector<RecordSessionPtr>::iterator it = mRecordSessions.begin();
  for(; it != mRecordSessions.end(); ++it)
  {
    QDomElement sessionNode = doc.createElement("recordSession");
    (*it)->addXml(sessionNode);
    sessionsNode.appendChild(sessionNode);
  }
  base.appendChild(sessionsNode);
}

void StateManager::parseXml(QDomNode& dataNode)
{
  QDomNode recordsessionsNode = dataNode.namedItem("recordSessions");
  QDomElement recodesessionNode = recordsessionsNode.firstChildElement("recordSession");
  for (; !recodesessionNode.isNull(); recodesessionNode = recodesessionNode.nextSiblingElement("recordSession"))
  {
    RecordSessionPtr session(new RecordSession(0,0,""));
    session->parseXml(recodesessionNode);
    this->addRecordSession(session);
  }
}

} //namespace cx
