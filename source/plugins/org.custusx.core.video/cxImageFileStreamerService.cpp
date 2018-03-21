/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxImageFileStreamerService.h"

#include "cxStringProperty.h"
#include "cxDoubleProperty.h"
#include "cxBoolProperty.h"
#include "cxMHDImageStreamer.h"
#include "cxLocalServerStreamerServer.h"
#include "cxTypeConversions.h"
#include "cxFilePathProperty.h"


namespace cx
{


///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

QString ImageFileStreamerService::getName()
{
	return "3D Image File";
}

QString ImageFileStreamerService::getType() const
{
	return "image_file_streamer";
}

std::vector<PropertyPtr> ImageFileStreamerService::getSettings(QDomElement root)
{
	std::vector<PropertyPtr> retval;

	std::vector<PropertyPtr> opencvArgs = ImageStreamerDummyArguments().getSettings(root);
	std::copy(opencvArgs.begin(), opencvArgs.end(), back_inserter(retval));

	std::vector<PropertyPtr> localsvrArgs = LocalServerStreamerArguments().getSettings(root);
	std::copy(localsvrArgs.begin(), localsvrArgs.end(), back_inserter(retval));

	return retval;
}

StreamerPtr ImageFileStreamerService::createStreamer(QDomElement root)
{
//	std::cout << "filecontent create streamer\n" << root.ownerDocument().toString().toStdString() << std::endl;

	StringMap args = ImageStreamerDummyArguments().convertToCommandLineArguments(root);
	StreamerPtr localServerStreamer = LocalServerStreamer::createStreamerIfEnabled(root, args);

	if (localServerStreamer)
	{
		return localServerStreamer;
	}
	else
	{
		boost::shared_ptr<DummyImageStreamer> streamer(new DummyImageStreamer());

		QString filename = ImageStreamerDummyArguments().getFilenameOption(root)->getValue();
		bool secondary = ImageStreamerDummyArguments().getSecondaryOption(root)->getValue();
		streamer->initialize(filename, secondary);
		return streamer;
	}
}

} // namespace cx

