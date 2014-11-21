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

#include "cxStringDataAdapterXml.h"
#include "cxDoubleDataAdapterXml.h"
#include "cxBoolDataAdapterXml.h"
#include "cxIGTLinkClientStreamer.h"
#include "cxMHDImageStreamer.h"
#include "cxOpenCVStreamerService.h"

namespace cx
{

QString ImageFileStreamerService::getName()
{
	return "ImageFile_new";
}

std::vector<DataAdapterPtr> ImageFileStreamerService::getSettings(QDomElement root)
{
	std::vector<DataAdapterPtr> retval;
	std::vector<DataAdapterPtr> opencvArgs = ImageStreamerDummyArguments().getSettings(root);
	std::copy(opencvArgs.begin(), opencvArgs.end(), back_inserter(retval));
	retval.push_back(this->getRunLocalServerOption(root));
	retval.push_back(this->getLocalServerNameOption(root));
	return retval;
}

BoolDataAdapterPtr ImageFileStreamerService::getRunLocalServerOption(QDomElement root)
{
	BoolDataAdapterXmlPtr retval;
	bool defaultValue = false;
	retval = BoolDataAdapterXml::initialize("runlocalserver", "Run Local Server",
											"Run streamer in a separate process",
											defaultValue, root);
	retval->setAdvanced(false);
	retval->setGroup("Connection");
	return retval;
}
StringDataAdapterPtr ImageFileStreamerService::getLocalServerNameOption(QDomElement root)
{
	StringDataAdapterXmlPtr retval;
	QString defaultValue = "OpenIGTLinkServer";
	retval = StringDataAdapterXml::initialize("localservername", "Server Name",
											  "Name of server executable, used only if Run Local Server is set.",
											  defaultValue, root);
	retval->setAdvanced(false);
	retval->setGroup("Connection");
	return retval;
}

StreamerPtr ImageFileStreamerService::createStreamer(QDomElement root)
{
	bool useLocalServer = this->getRunLocalServerOption(root)->getValue();
	StringMap args = ImageStreamerDummyArguments().convertToCommandLineArguments(root);

	if (useLocalServer)
	{
		QStringList cmdlineArguments;
		for (StringMap::iterator i=args.begin(); i!=args.end(); ++i)
			cmdlineArguments << i->first << i->second;

		QString localServer = this->getLocalServerNameOption(root)->getValue();
		boost::shared_ptr<LocalServerStreamer> streamer;
		streamer.reset(new LocalServerStreamer(localServer, cmdlineArguments.join(" ")));

		return streamer;
	}
	else
	{
		boost::shared_ptr<DummyImageStreamer> streamer(new DummyImageStreamer());
		streamer->initialize(args["filename"], args.count("secondary"));
		return streamer;
	}
}

ReceiverPtr ImageFileStreamerService::createReceiver(QDomElement root)
{
	return ReceiverPtr();
}

} // namespace cx

