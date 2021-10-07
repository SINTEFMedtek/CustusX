/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXAIRWAYSFILTERSERVICE_H_
#define CXAIRWAYSFILTERSERVICE_H_

#include "org_custusx_filter_airways_Export.h"

#include "cxFilter.h"
#include "cxFilterImpl.h"

#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>

#include "cxPatientModelService.h"

#include "cxStringProperty.h"
#include "cxBoolProperty.h"

#include "FAST/Importers/ImageFileImporter.hpp"
#include "FAST/Algorithms/LungSegmentation/LungSegmentation.hpp"
#include "FAST/Algorithms/AirwaySegmentation/AirwaySegmentation.hpp"

class ctkPluginContext;

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkPolyData> vtkPolyDataPtr;

namespace cx {

/**
 * Filter for airway segmentation and centerline extraction of a CT volume.
 *
 * Algorithm written by Erik Smistad.
 *
 * 2020-01-15: Added segmentation of blood vessels and lungs.
 * Erlend Fagertun hofstad
 *
 */
class org_custusx_filter_airways_EXPORT AirwaysFilter : public FilterImpl
{
Q_OBJECT
Q_INTERFACES(cx::Filter)

public:
	AirwaysFilter(VisServicesPtr services);
	virtual ~AirwaysFilter();

	virtual QString getType() const;
	virtual QString getName() const;
	virtual QString getHelp() const;

	bool preProcess();
	virtual bool execute();
	virtual bool postProcess();

	void setAirwaySegmentation(bool airwaySegmentation);
	void setColoringAirways(bool coloringAirways);
	void setVesselSegmentation(bool vesselSegmentation);

protected:
	void segmentAirways(fast::ImageFileImporter::pointer importerPtr);
	bool extractAirways(fast::AirwaySegmentation::pointer airwaySegmentationPtr);
	void segmentLungs(fast::ImageFileImporter::pointer importerPtr);
	void segmentVessels(fast::ImageFileImporter::pointer importerPtr);
	bool extractBloodVessels(fast::LungSegmentation::pointer lungSegmentationPtr);
	bool extractLungs(fast::LungSegmentation::pointer lungSegmentationPtr);
	bool postProcessAirways();
	bool postProcessLungs();
	bool postProcessVessels();
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();

private:
	static Vector3D getSeedPointFromTool(SpaceProviderPtr spaceProvider, DataPtr image);
	static bool isSeedPointInsideImage(Vector3D, DataPtr);
	BoolPropertyPtr getManualSeedPointOption(QDomElement root);
	BoolPropertyPtr getAirwaySegmentationOption(QDomElement root);
	BoolPropertyPtr getAirwayTubesGenerationOption(QDomElement root);
	BoolPropertyPtr getColoredAirwaysOption(QDomElement root);
	BoolPropertyPtr getLungSegmentationOption(QDomElement root);
	BoolPropertyPtr getVesselSegmentationOption(QDomElement root);
	BoolPropertyPtr getVesselCenterlineOption(QDomElement root);
	BoolPropertyPtr getVesselVolumeOption(QDomElement root);
	void createAirwaysFromCenterline();
	void createColoredAirways();

	vtkImageDataPtr mAirwaySegmentationOutput;
	vtkPolyDataPtr mAirwayCenterlineOutput;
	vtkImageDataPtr mLungSegmentationOutput;
	vtkImageDataPtr mBloodVesselSegmentationOutput;
	vtkPolyDataPtr mBloodVesselCenterlineOutput;
	Transform3D mTransformation;
	ImagePtr mInputImage;
	Vector3D seedPoint;
	bool mDefaultStraightCLTubesOption;
	BoolPropertyPtr mManualSeedPointOption;
	BoolPropertyPtr mAirwaySegmentationOption;
	BoolPropertyPtr mAirwayTubesGenerationOption;
	BoolPropertyPtr mColoredAirwaysOption;
	BoolPropertyPtr mLungSegmentationOption;
	BoolPropertyPtr mVesselSegmentationOption;
	BoolPropertyPtr mVesselCenterlineOption;
	BoolPropertyPtr mVesselVolumeOption;
};
typedef boost::shared_ptr<class AirwaysFilter> AirwaysFilterPtr;
typedef boost::shared_ptr<class GenericScriptFilter> GenericScriptFilterPtr;

} /* namespace cx */
#endif /* CXTUBESEGMENTATIONFILTERSERVICE_H_ */
