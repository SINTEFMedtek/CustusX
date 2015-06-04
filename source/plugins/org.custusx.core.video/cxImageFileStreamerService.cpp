/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/
#include "cxImageFileStreamerService.h"

#include "cxStringProperty.h"
#include "cxDoubleProperty.h"
#include "cxBoolProperty.h"
#include "cxIGTLinkClientStreamer.h"
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

