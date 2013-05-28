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

#ifndef CXIMAGESTREAMER_H_
#define CXIMAGESTREAMER_H_

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>

#include <map>
#include "boost/shared_ptr.hpp"
#include "cxGrabberSender.h"

namespace cx
{

/**\brief
 *
 * \ingroup cxGrabberServer
 *
 * \date May 22, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class Streamer : public QObject
{
	Q_OBJECT

public:
	Streamer();
	virtual ~Streamer(){};

	virtual bool startStreaming(SenderPtr sender) = 0;
	virtual void stopStreaming() = 0;

	virtual QString getType() = 0;

	void setSendInterval(int milliseconds); ///< how often an image should be sent (in milliseconds)
	int getSendInterval() const; ///< how often an image should be sent (in milliseconds)

protected slots:
	virtual void streamSlot() = 0;

protected:
	void setInitialized(bool initialized);
	bool isInitialized();
	void createSendTimer(bool singleshot = false);
	bool isReadyToSend();
	SenderPtr mSender;
	QTimer* mSendTimer;

private:
	int mSendInterval; ///< how often an image should be sent (in milliseconds)
	bool mInitialized;

};
typedef boost::shared_ptr<Streamer> StreamerPtr;


typedef std::map<QString, QString> StringMap;
/**\brief
 *
 * \ingroup cxGrabberServer
 *
 * \date Apr 17, 2012
 * \author Christian Askeland, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 * \author Janne Beate Bakeng, SINTEF
 */
class ImageStreamer : public Streamer
{
	Q_OBJECT
public:
	ImageStreamer() {}
	virtual ~ImageStreamer() {}

protected:

private:

};
typedef boost::shared_ptr<ImageStreamer> ImageStreamerPtr;

/**\brief
 *
 * \ingroup cxGrabberServer
 *
 * \date May 22, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class CommandLineStreamer : public Streamer
{
	Q_OBJECT

public:
	CommandLineStreamer(){};
	virtual ~CommandLineStreamer(){};

	virtual QStringList getArgumentDescription() = 0;

	virtual void initialize(StringMap arguments);

protected slots:
	virtual void streamSlot() {std::cout << "THIS SHOULD NOT HAPPEN...." << std::endl;};


protected:
	StringMap mArguments;
};
typedef boost::shared_ptr<CommandLineStreamer> CommandLineStreamerPtr;
}

#endif /* CXIMAGESTREAMER_H_ */
