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
    static QString getNameSuffix();
	static QString getNameSuffixStraight();
	static QString getNameSuffixTubes();
	void setDefaultStraightCLTubesOption(bool defaultStraightCLTubesOption);

    bool preProcess();
	virtual bool execute();
	virtual bool postProcess();

protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();

private:
    static Vector3D getSeedPointFromTool(SpaceProviderPtr spaceProvider, DataPtr image);
    static bool isSeedPointInsideImage(Vector3D, DataPtr);
	BoolPropertyPtr getManualSeedPointOption(QDomElement root);
    BoolPropertyPtr getLungSegmentationOption(QDomElement root);
	BoolPropertyPtr getStraightCLTubesOption(QDomElement root);
	void createStraightCL();
	void createTubes();
	vtkImageDataPtr mAirwaySegmentationOutput;
    vtkImageDataPtr mLungSegmentationOutput;
	vtkPolyDataPtr mCenterlineOutput;
	Transform3D mTransformation;
	ImagePtr mInputImage;
    Vector3D seedPoint;
	bool mDefaultStraightCLTubesOption;
};
typedef boost::shared_ptr<class AirwaysFilter> AirwaysFilterPtr;

} /* namespace cx */
#endif /* CXTUBESEGMENTATIONFILTERSERVICE_H_ */
