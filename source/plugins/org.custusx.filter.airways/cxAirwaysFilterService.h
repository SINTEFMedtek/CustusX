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

class ctkPluginContext;

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkPolyData> vtkPolyDataPtr;

namespace cx {

/**
 * Filter for airway segmentation and centerline extraction of a CT volume.
 *
 * Algorithm written by Erik Smistad.
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
    static QString getNameSuffixCenterline();
	static QString getNameSuffixAirways();
	static QString getNameSuffixTubes();
	static QString getNameSuffixLungs();
	static QString getNameSuffixVessels();

    bool preProcess();
	virtual bool execute();
	virtual bool postProcess();

protected:
	bool segmentAirways(fast::ImageFileImporter::pointer importerPtr);
	void segmentLungsAndVessels(fast::ImageFileImporter::pointer importerPtr);
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
    BoolPropertyPtr getLungSegmentationOption(QDomElement root);
    BoolPropertyPtr getVesselSegmentationOption(QDomElement root);
    void createAirwaysFromCenterline();
	vtkImageDataPtr mAirwaySegmentationOutput;
	vtkPolyDataPtr mAirwayCenterlineOutput;
    vtkImageDataPtr mLungSegmentationOutput;
    vtkImageDataPtr mBloodVesselSegmentationOutput;
	vtkPolyDataPtr mBloodVesselCenterlineOutput;
	Transform3D mTransformation;
	ImagePtr mInputImage;
    Vector3D seedPoint;
	bool mDefaultStraightCLTubesOption;
};
typedef boost::shared_ptr<class AirwaysFilter> AirwaysFilterPtr;

} /* namespace cx */
#endif /* CXTUBESEGMENTATIONFILTERSERVICE_H_ */
