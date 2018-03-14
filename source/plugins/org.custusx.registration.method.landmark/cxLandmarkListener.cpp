/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxLandmarkListener.h"
#include "cxLandmarkRep.h"
#include "cxRegistrationService.h"
#include "cxViewService.h"
#include "cxRepContainer.h"

namespace cx
{

LandmarkListener::LandmarkListener(RegServicesPtr services) :
	mServices(services),
	mImage2Image(false),
	mUseOnlyOneSourceUpdatedFromOutside(false)
{
	mFixedLandmarkSource = ImageLandmarksSource::New();
	mMovingLandmarkSource = ImageLandmarksSource::New();

	connect(mServices->registration().get(), &RegistrationService::fixedDataChanged, this, &LandmarkListener::updateFixed);
	connect(mServices->registration().get(), &RegistrationService::movingDataChanged, this, &LandmarkListener::updateMoving);
}

LandmarkListener::~LandmarkListener()
{
	disconnect(mServices->registration().get(), &RegistrationService::fixedDataChanged, this, &LandmarkListener::updateFixed);
	disconnect(mServices->registration().get(), &RegistrationService::movingDataChanged, this, &LandmarkListener::updateMoving);
}

void LandmarkListener::useI2IRegistration(bool useI2I)
{
	mImage2Image = useI2I;
}

void LandmarkListener::useOnlyOneSourceUpdatedFromOutside(bool useOnlyOneSourceUpdatedFromOutside)
{
	mUseOnlyOneSourceUpdatedFromOutside = useOnlyOneSourceUpdatedFromOutside;
	disconnect(mServices->registration().get(), &RegistrationService::fixedDataChanged, this, &LandmarkListener::updateFixed);
	disconnect(mServices->registration().get(), &RegistrationService::movingDataChanged, this, &LandmarkListener::updateMoving);
}


void LandmarkListener::updateFixed()
{
	mFixedLandmarkSource->setData(mServices->registration()->getFixedData());
}

void LandmarkListener::updateMoving()
{
	mMovingLandmarkSource->setData(mServices->registration()->getMovingData());
}

void LandmarkListener::setLandmarkSource(DataPtr data)
{
	if(!mUseOnlyOneSourceUpdatedFromOutside)
		return;
	mFixedLandmarkSource->setData(data);
}

DataPtr LandmarkListener::getLandmarkSource()
{
	return mFixedLandmarkSource->getData();
}

void LandmarkListener::showRep()
{
	if(!mServices->view()->get3DView(0, 0))
		return;

	LandmarkRepPtr rep = mServices->view()->get3DReps(0, 0)->findFirst<LandmarkRep>();

	if (rep)
	{
		rep->setPrimarySource(mFixedLandmarkSource);
		rep->setSecondaryColor(QColor::fromRgbF(0, 0.6, 0.8));
//		rep->setSecondaryColor(QColor::fromRgbF(0, 0.9, 0.5));

		if (mUseOnlyOneSourceUpdatedFromOutside)
			rep->setSecondarySource(LandmarksSourcePtr());//Only show one source
		else if(mImage2Image)
			rep->setSecondarySource(mMovingLandmarkSource);//I2I reg
		else
			rep->setSecondarySource(PatientLandmarksSource::New(mServices->patient()));//I2P reg
	}
}

void LandmarkListener::hideRep()
{
	if(!mServices->view()->get3DView(0, 0))
		return;

	LandmarkRepPtr rep = mServices->view()->get3DReps(0, 0)->findFirst<LandmarkRep>();
	if (rep)
	{
		rep->setPrimarySource(LandmarksSourcePtr());
		rep->setSecondarySource(LandmarksSourcePtr());
	}
}

} //cx
