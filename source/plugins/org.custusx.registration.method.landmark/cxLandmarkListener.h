/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXLANDMARKLISTENER_H
#define CXLANDMARKLISTENER_H

#include "org_custusx_registration_method_landmarkExport.h"

#include <QObject>
#include <boost/shared_ptr.hpp>
#include "cxRegServices.h"

namespace cx
{

typedef boost::shared_ptr<class LandmarkListener> LandmarkListenerPtr;
typedef boost::shared_ptr<class ImageLandmarksSource> ImageLandmarksSourcePtr;
typedef boost::shared_ptr<class Data> DataPtr;


class org_custusx_registration_method_landmark_EXPORT LandmarkListener : public QObject
{
Q_OBJECT

public:
	LandmarkListener(RegServicesPtr services);
	~LandmarkListener();
	void setLandmarkSource(DataPtr data);
	DataPtr getLandmarkSource();
	void useOnlyOneSourceUpdatedFromOutside(bool useOnlyOneSourceUpdatedFromOutside = true);
	void useI2IRegistration(bool useI2I = true);
public slots:
	void showRep();
	void hideRep();
private slots:
	void updateFixed();
	void updateMoving();
private:
	RegServicesPtr mServices;
	bool mImage2Image;
	bool mUseOnlyOneSourceUpdatedFromOutside;
	ImageLandmarksSourcePtr mFixedLandmarkSource;
	ImageLandmarksSourcePtr mMovingLandmarkSource;
};

} //cx

#endif // CXLANDMARKLISTENER_H
