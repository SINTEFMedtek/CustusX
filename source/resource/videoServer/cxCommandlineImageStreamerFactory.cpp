/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxCommandlineImageStreamerFactory.h"

#include "cxLogger.h"
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

	CommandLineStreamerPtr streamer = this->getImageSender(type);

	if (streamer)
		reportSuccess("Created sender of type: "+type);
	else
	{
		reportError("Failed to create sender based on type: "+type);
		return streamer;
	}

	streamer->initialize(args);
	return streamer;
}


CommandlineImageStreamerFactory::CommandlineImageStreamerFactory()
{
#ifdef CX_USE_OpenCV
	mCommandLineStreamers.push_back(CommandLineStreamerPtr(new ImageStreamerOpenCV()));
#endif
	mCommandLineStreamers.push_back(DummyImageStreamerPtr(new DummyImageStreamer()));
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
	return retval;
}

QStringList CommandlineImageStreamerFactory::getArgumentDescription(QString type) const
{
	for (unsigned i=0; i< mCommandLineStreamers.size(); ++i)
		if (mCommandLineStreamers[i]->getType()==type)
			return mCommandLineStreamers[i]->getArgumentDescription();
	return QStringList();
}

CommandLineStreamerPtr CommandlineImageStreamerFactory::getImageSender(QString type)
{
	for (unsigned i=0; i< mCommandLineStreamers.size(); ++i)
		if (mCommandLineStreamers[i]->getType()==type)
			return mCommandLineStreamers[i];
	return CommandLineStreamerPtr();
}


}
