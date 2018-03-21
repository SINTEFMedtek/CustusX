/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxStringPropertyActiveVideoSource.h"

#include "cxVideoService.h"
#include "cxVideoSource.h"

namespace cx
{

StringPropertyActiveVideoSource::StringPropertyActiveVideoSource(VideoServicePtr service) :
	mService(service)
{
	connect(mService.get(), &VideoService::activeVideoSourceChanged, this, &Property::changed);
}

QString StringPropertyActiveVideoSource::getDisplayName() const
{
	return "Stream";
}

bool StringPropertyActiveVideoSource::setValue(const QString& value)
{
	if (value == this->getValue())
		return false;
	mService->setActiveVideoSource(value);
	emit changed();
	return true;
}

QString StringPropertyActiveVideoSource::getValue() const
{
	return mService->getActiveVideoSource()->getUid();
}

QStringList StringPropertyActiveVideoSource::getValueRange() const
{
	std::vector<VideoSourcePtr> sources = mService->getVideoSources();
	QStringList retval;
	for (unsigned i=0; i<sources.size(); ++i)
		retval << sources[i]->getUid();
	return retval;
}

QString StringPropertyActiveVideoSource::getHelp() const
{
	return "Select the active video source.";
}


} // namespace cx
