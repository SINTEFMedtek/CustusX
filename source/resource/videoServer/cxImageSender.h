// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXIMAGESENDER_H_
#define CXIMAGESENDER_H_

#include <QObject>
#include <QString>
#include <QStringList>
//#include <QTcpSocket>
#include <QTimer>

#include <map>
#include "boost/shared_ptr.hpp"
#include "cxGrabberSender.h"

namespace cx
{

typedef std::map<QString, QString> StringMap;
typedef boost::shared_ptr<class ImageStreamer> ImageStreamerPtr;

/**\brief Interface for objects that emit an image stream to IGTLink
 *
 * \ingroup cxGrabberServer
 *
 * \date Apr 17, 2012
 * \author Christian Askeland, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 * \author Janne Beate Bakeng, SINTEF
 */
class ImageStreamer : public QObject
{
	Q_OBJECT
public:
	ImageStreamer(QObject* parent = NULL) : QObject(parent), mSendTimer(0) {}
	virtual ~ImageStreamer() {}

	virtual void initialize(StringMap arguments);
	virtual bool startStreaming(GrabberSenderPtr sender) = 0;
	virtual void stopStreaming() = 0;

	virtual QString getType() = 0;
	virtual QStringList getArgumentDescription() = 0;

protected:
	GrabberSenderPtr mSender;
	QTimer* mSendTimer;
	StringMap mArguments;
};

}

#endif /* CXIMAGESENDER_H_ */
