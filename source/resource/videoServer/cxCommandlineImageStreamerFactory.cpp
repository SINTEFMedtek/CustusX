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

#include "cxCommandlineImageStreamerFactory.h"

#include "cxReporter.h"
#include "cxImageStreamerOpenCV.h"
#include "cxMHDImageStreamer.h"
#include "cxImageStreamerSonix.h"
#include "cxConfig.h"

namespace cx
{

StringMap extractCommandlineOptions(QStringList cmdline)
{
	StringMap retval;

	for (int i = 0; i < cmdline.size(); ++i)
	{
		if (!cmdline[i].startsWith("--"))
			continue;
		QString name, val;
		name = cmdline[i].remove(0, 2);
		if (i + 1 < cmdline.size())
			val = cmdline[i + 1];
		retval[name] = val;
	}

	return retval;
}


///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

StreamerPtr CommandlineImageStreamerFactory::getFromArguments(StringMap args)
{
	QString type = this->getDefaultSenderType();
	if (args.count("type"))
		type = args["type"];

	StreamerPtr streamer = this->getImageSender(type);

	if (streamer)
		reportSuccess("Created sender of type: "+type);
	else
	{
		reportError("Failed to create sender based on type: "+type);
		return streamer;
	}

	CommandLineStreamerPtr commandlinestreamer = boost::dynamic_pointer_cast<CommandLineStreamer>(streamer);
	if(commandlinestreamer)
		commandlinestreamer->initialize(args);

	DummyImageStreamerPtr mhdimagestreamer = boost::dynamic_pointer_cast<DummyImageStreamer>(streamer);
	QString filename = args["filename"];
	bool secondary = args.count("secondary") ? true : false;
	if(mhdimagestreamer)
		mhdimagestreamer->initialize(filename, secondary);

	return streamer;
}


CommandlineImageStreamerFactory::CommandlineImageStreamerFactory()
{
#ifdef CX_WIN32
	mCommandLineStreamers.push_back(CommandLineStreamerPtr(new ImageStreamerSonix()));
#endif
#ifdef CX_USE_OpenCV
	mCommandLineStreamers.push_back(CommandLineStreamerPtr(new ImageStreamerOpenCV()));
#endif
	mImageStreamers.push_back(DummyImageStreamerPtr(new DummyImageStreamer()));
}

QString CommandlineImageStreamerFactory::getDefaultSenderType() const
{
	return this->getSenderTypes().front();
}

QStringList CommandlineImageStreamerFactory::getSenderTypes() const
{
	QStringList retval;
	for (unsigned i=0; i< mCommandLineStreamers.size(); ++i)
		retval << mCommandLineStreamers[i]->getType();
	for (unsigned i=0; i< mImageStreamers.size(); ++i)
		retval << mImageStreamers[i]->getType();
	return retval;
}

QStringList CommandlineImageStreamerFactory::getArgumentDescription(QString type) const
{
	QStringList retval;
	for (unsigned i=0; i< mCommandLineStreamers.size(); ++i)
	{
		if (mCommandLineStreamers[i]->getType()==type)
			return mCommandLineStreamers[i]->getArgumentDescription();
	}
	return retval;
}

StreamerPtr CommandlineImageStreamerFactory::getImageSender(QString type)
{
	for (unsigned i=0; i< mCommandLineStreamers.size(); ++i)
	{
		if (mCommandLineStreamers[i]->getType()==type)
			return mCommandLineStreamers[i];
	}
	for (unsigned i=0; i< mImageStreamers.size(); ++i)
	{
		if (mImageStreamers[i]->getType()==type)
			return mImageStreamers[i];
	}
	return StreamerPtr();
}


}
