/*
 * cxImageSenderFactory.cpp
 *
 *  Created on: Aug 9, 2011
 *      Author: christiana
 */

#include "cxImageSenderFactory.h"
#include "cxImageSenderOpenCV.h"
#include "cxImageSenderFile.h"
#include "cxImageSenderSonix.h"

namespace cx
{

int convertStringWithDefault(QString text, int def)
{
	bool ok = true;
	int retval = text.toInt(&ok);
	if (ok)
		return retval;
	return def;
}

StringMap extractCommandlineOptions(QStringList cmdline)
{
	StringMap retval;

	for (int i=0; i< cmdline.size(); ++i)
	{
		if (!cmdline[i].startsWith("--"))
			continue;
		QString name, val;
		name = cmdline[i].remove(0,2);
		if (i+1 < cmdline.size())
		{
			val = cmdline[i+1];
		}
		retval[name] = val;
	}

	return retval;
}


QStringList ImageSenderFactory::getSenderTypes() const
{
	QStringList retval;

#ifdef USE_OpenCV
	retval << ImageSenderOpenCV::getType();
#endif
#ifdef WIN32
  retval << ImageSenderSonix::getType();
#endif

	retval << MHDImageSender::getType();
	return retval;
}

QStringList ImageSenderFactory::getArgumentDescription(QString type) const
{
#ifdef USE_OpenCV
	if (type==ImageSenderOpenCV::getType())
		return ImageSenderOpenCV::getArgumentDescription();
#endif
#ifdef WIN32
  if (type==ImageSenderSonix::getType())
    return ImageSenderSonix::getArgumentDescription();
#endif

	if (type==MHDImageSender::getType())
		return MHDImageSender::getArgumentDescription();


	return QStringList();
}

QObject* ImageSenderFactory::createSender(QString type, QTcpSocket* socket, StringMap arguments) const
{
#ifdef USE_OpenCV
	if (type==ImageSenderOpenCV::getType())
		return new ImageSenderOpenCV(socket, arguments);
#endif
#ifdef WIN32
  if (type==ImageSenderSonix::getType())
    return new ImageSenderSonix(socket, arguments);
#endif

	if (type==MHDImageSender::getType())
		return new MHDImageSender(socket, arguments);


	// default:
#ifdef WIN32
  return new ImageSenderSonix(socket, arguments);
#elif USE_OpenCV
  return new ImageSenderOpenCV(socket, arguments);
#else
	return NULL;
#endif
}


}
