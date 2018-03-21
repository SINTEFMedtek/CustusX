/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxStateService.h"

#include <math.h>
#include <QColor>
#include "cxStateServiceNull.h"
#include "cxNullDeleter.h"
#include "cxVLCRecorder.h"
#include "cxDefinitions.h"

namespace cx
{

Desktop::Desktop()
{
}

Desktop::Desktop(QString layout, QByteArray mainwindowstate) :
				mLayoutUid(layout), mMainWindowState(mainwindowstate)
{
}

void Desktop::addPreset(QString name, int pos, bool tabbed)
{
	Preset item;
	item.name = name;
	item.position = pos;
	item.tabbed = tabbed;
	mPresets.push_back(item);
}


StateServicePtr StateService::getNullObject()
{
	static StateServicePtr mNull;
	if (!mNull)
		mNull.reset(new StateServiceNull, null_deleter());
	return mNull;
}

/**Enter all default Settings here.
 *
 */
void StateService::fillDefaultSettings()
{
	this->fillDefault("Automation/autoStartTracking", true);
	this->fillDefault("Automation/autoStartStreaming", true);
	this->fillDefault("Automation/autoReconstruct", true);
	this->fillDefault("Automation/autoSelectActiveTool", true);
	this->fillDefault("Automation/autoSave", true);
	this->fillDefault("Automation/autoLoadRecentPatient", true);
	this->fillDefault("Automation/autoLoadRecentPatientWithinHours", 8);
	this->fillDefault("Automation/autoShowNewData", false);
	this->fillDefault("Automation/autoShowNewDataInViewGroup0", true);
	this->fillDefault("Automation/autoShowNewDataInViewGroup1", false);
	this->fillDefault("Automation/autoShowNewDataInViewGroup3", false);
	this->fillDefault("Automation/autoShowNewDataInViewGroup2", false);
	this->fillDefault("Automation/autoShowNewDataInViewGroup4", false);
	this->fillDefault("Automation/autoResetCameraToSuperiorViewWhenAutoShowingNewData", false);
	this->fillDefault("Automation/autoCenterToImageCenterViewWhenAutoShowingNewData", false);
	this->fillDefault("Automation/autoDeleteDICOMDatabase", true);

	this->fillDefault("TrackingPositionFilter/enabled", false);
	this->fillDefault("TrackingPositionFilter/cutoffFrequency", 3.0);

	this->fillDefault("renderingInterval", 33);
	this->fillDefault("backgroundColor", QColor(30,60,70)); // a dark, grey-blue hue
	this->fillDefault("vlcPath", vlc()->getVLCPath());
	this->fillDefault("globalPatientNumber", 1);
	this->fillDefault("Ultrasound/acquisitionName", "US-Acq");
	this->fillDefault("Ultrasound/8bitAcquisitionData", false);
	this->fillDefault("Ultrasound/CompressAcquisition", true);
	this->fillDefault("View3D/sphereRadius", 1.0);
	this->fillDefault("View3D/labelSize", 2.5);
	this->fillDefault("Navigation/anyplaneViewOffset", 0.25);
	this->fillDefault("Navigation/followTooltip", true);
	this->fillDefault("Navigation/followTooltipBoundary", 0.1);

	this->fillDefault("View2D/toolColor", QColor::fromRgbF(0.25, 0.87, 0.16));
	this->fillDefault("View/toolTipPointColor", QColor::fromRgbF(0.96, 0.87, 0.17));
	this->fillDefault("View/toolOffsetPointColor", QColor::fromRgbF(0.96, 0.87, 0.17));
	this->fillDefault("View/toolOffsetLineColor", QColor::fromRgbF(1.0, 0.8, 0.0));
	this->fillDefault("View2D/toolCrossHairColor", QColor::fromRgbF(1.0, 0.8, 0.0));
	this->fillDefault("View2D/showToolCrosshair", true);
	this->fillDefault("View3D/showManualTool", false);
	this->fillDefault("View2D/showManualTool", true);

	this->fillDefault("showSectorInRTView", true);
	this->fillDefault("View/showOrientationAnnotation", true);
	this->fillDefault("View3D/stereoType", stFRAME_SEQUENTIAL);
	this->fillDefault("View3D/eyeAngle", 4.0);
	this->fillDefault("View/showDataText", true);
	this->fillDefault("View/showLabels", true);
	this->fillDefault("View/showMetricNamesInCorner", false);
	this->fillDefault("View3D/annotationModelSize", 0.2);
	this->fillDefault("View3D/annotationModel", "woman.stl");
	this->fillDefault("View3D/depthPeeling", false);
	this->fillDefault("View3D/ImageRender3DVisualizer", "vtkGPUVolumeRayCastMapper");
	this->fillDefault("View3D/maxRenderSize", 10 * pow(10.0,6));
	this->fillDefault("View/shadingOn", true);

	this->fillDefault("Gui/showMenuBar", true);

	this->fillDefault("stillUpdateRate", 0.001);

	this->fillDefault("View2D/useGPU2DRendering", true);
	this->fillDefault("View2D/useLinearInterpolationIn2DRendering", true);

	this->fillDefault("optimizedViews", true);
	this->fillDefault("smartRender", true);

	this->fillDefault("IGSTKDebugLogging", false);
	this->fillDefault("giveManualToolPhysicalProperties", false);
	this->fillDefault("renderSpeedLogging", false);

	this->fillDefault("applyTransferFunctionPresetsToAll", false);

	this->fillDefault("USsimulation/type", "Original data");
	this->fillDefault("USsimulation/volume", "");
	this->fillDefault("USsimulation/gain", 0.70);

	this->fillDefault("Dicom/ShowAdvanced", false);
	this->fillDefault("Landmarks/ShowAdvanced", false);
}


} //namespace cx
