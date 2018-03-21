/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXIMAGESENDERFACTORY_H_
#define CXIMAGESENDERFACTORY_H_

#include "cxGrabberExport.h"

#include <QObject>
#include <QStringList>
#include <map>
#include <vector>
#include "cxStreamer.h"

class QTcpSocket;

namespace cx
{

typedef std::map<QString, QString> StringMap;
cxGrabber_EXPORT StringMap extractCommandlineOptions(QStringList cmdline);


/**
 * \brief Factory class for creating streamer objects.
 *
 * \ingroup cx_resource_videoserver
 * \author Christian Askeland, SINTEF
 * \author Janne Beate Bakeng, SINTEF
 * \date Aug 9, 2011
 */
class cxGrabber_EXPORT CommandlineImageStreamerFactory
{
public:
	CommandlineImageStreamerFactory();
	QString getDefaultSenderType() const;
	QStringList getSenderTypes() const; ///< all available sender types
	QStringList getArgumentDescription(QString type) const; ///< arguments for one streamer
	CommandLineStreamerPtr getImageSender(QString type);
	StreamerPtr getFromArguments(StringMap args);

private:
	std::vector<CommandLineStreamerPtr> mCommandLineStreamers;
};

}

#endif /* CXIMAGESENDERFACTORY_H_ */
