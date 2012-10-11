/*
 * cxImageSenderFactory.cpp
 *
 *  \date Aug 9, 2011
 *      \author christiana
 */

#include "cxImageSenderFactory.h"
#include "cxImageSenderOpenCV.h"
#include "cxImageSenderFile.h"
#include "cxImageSenderSonix.h"
#include "cxImageSenderGE.h"

namespace cx
{

int convertStringWithDefault(QString text, int def)
{
	bool ok = true;
	int retval = text.toInt(&ok,0);
	if (ok)
		return retval;
	return def;
}

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
		{
			val = cmdline[i + 1];
		}
		retval[name] = val;
	}

	return retval;
}


///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

ImageSenderPtr ImageSenderFactory::getFromArguments(StringMap args)
{
	QString type = this->getDefaultSenderType();
	if (args.count("type"))
		type = args["type"];

	ImageSenderPtr retval = this->getImageSender(type);

	if (retval)
	{
		std::cout << "Success: Created sender of type: " << type.toStdString() << std::endl;
	}
	else
	{
		std::cout << "Error: Failed to create sender based on type: " << type.toStdString() << std::endl;
	}

	retval->initialize(args);
	return retval;
}


ImageSenderFactory::ImageSenderFactory()
{
#ifdef CX_WIN32
	mAvailable.push_back(ImageSenderPtr(new ImageSenderSonix()));
#endif
#ifdef USE_OpenCV
	mAvailable.push_back(ImageSenderPtr(new ImageSenderOpenCV()));
#endif
#ifdef CX_USE_ISB_GE
	mAvailable.push_back(ImageSenderPtr(new ImageSenderGE()));
#endif
	mAvailable.push_back(ImageSenderPtr(new MHDImageSender()));
}

QString ImageSenderFactory::getDefaultSenderType() const
{
	// use the FIRST sender available
  QString retval = mAvailable.front()->getType();
  return retval;
	//return mAvailable.front()->getType();
}

QStringList ImageSenderFactory::getSenderTypes() const
{
	QStringList retval;
	for (unsigned i=0; i< mAvailable.size(); ++i)
		retval << mAvailable[i]->getType();
	return retval;
}

QStringList ImageSenderFactory::getArgumentDescription(QString type) const
{
	QStringList retval;
	for (unsigned i=0; i< mAvailable.size(); ++i)
	{
		if (mAvailable[i]->getType()==type)
			return mAvailable[i]->getArgumentDescription();
	}
	return retval;
}

ImageSenderPtr ImageSenderFactory::getImageSender(QString type)
{
	for (unsigned i=0; i< mAvailable.size(); ++i)
	{
		if (mAvailable[i]->getType()==type)
			return mAvailable[i];
	}
	return ImageSenderPtr();
}


}
