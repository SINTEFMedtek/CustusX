/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxOpenCVStreamerService.h"

#include "cxStringProperty.h"
#include "cxDoubleProperty.h"
#include "cxBoolProperty.h"
#include "cxImageStreamerOpenCV.h"
#include "cxUtilHelpers.h"

#include "QApplication"
#include <QDir>
#include "cxLocalServerStreamerServer.h"

namespace cx
{

QString OpenCVStreamerService::getName()
{
//	return "OpenCV";
	return "Video Grabber";
}

QString OpenCVStreamerService::getType() const
{
	return "open_cv_streamer";
}

std::vector<PropertyPtr> OpenCVStreamerService::getSettings(QDomElement root)
{
	std::vector<PropertyPtr> retval;
	std::vector<PropertyPtr> opencvArgs = ImageStreamerOpenCVArguments().getSettings(root);
	std::copy(opencvArgs.begin(), opencvArgs.end(), back_inserter(retval));

	std::vector<PropertyPtr> localsvrArgs = LocalServerStreamerArguments().getSettings(root);
	std::copy(localsvrArgs.begin(), localsvrArgs.end(), back_inserter(retval));

	return retval;
}

StreamerPtr OpenCVStreamerService::createStreamer(QDomElement root)
{
	StringMap args = ImageStreamerOpenCVArguments().convertToCommandLineArguments(root);
	StreamerPtr localServerStreamer = LocalServerStreamer::createStreamerIfEnabled(root, args);
	if (localServerStreamer)
	{
		return localServerStreamer;
	}

	else
	{
		boost::shared_ptr<ImageStreamerOpenCV> streamer(new ImageStreamerOpenCV());
		streamer->initialize(args);
		return streamer;
	}
}

} // namespace cx
