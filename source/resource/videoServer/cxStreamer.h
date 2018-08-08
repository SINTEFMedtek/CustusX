/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXSTREAMER_H_
#define CXSTREAMER_H_

#include "cxGrabberExport.h"

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>

#include <map>
#include "boost/shared_ptr.hpp"
#include <iostream>

namespace cx
{

typedef boost::shared_ptr<class Sender> SenderPtr;
typedef boost::shared_ptr<class Streamer> StreamerPtr;

/**\brief
 *
 * \ingroup cx_resource_videoserver
 *
 * \date May 22, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class cxGrabber_EXPORT Streamer : public QObject
{
	Q_OBJECT

public:
	Streamer();
	virtual ~Streamer(){}

	virtual void startStreaming(SenderPtr sender) = 0;
	virtual void stopStreaming() = 0;
	virtual bool isStreaming() = 0;

	void setSendInterval(int milliseconds); ///< how often an image should be sent (in milliseconds)
	int getSendInterval() const; ///< how often an image should be sent (in milliseconds)

signals:
	void stateChanged();

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

typedef std::map<QString, QString> StringMap;

/**\brief
 *
 *
 * \ingroup cx_resource_videoserver
 * \date May 22, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class cxGrabber_EXPORT CommandLineStreamer : public Streamer
{
	Q_OBJECT

public:
	CommandLineStreamer(){}
	virtual ~CommandLineStreamer(){}

	virtual QStringList getArgumentDescription() = 0;
	virtual QString getType() = 0;

	virtual void initialize(StringMap arguments);

protected slots:
	virtual void streamSlot() {std::cout << "THIS SHOULD NOT HAPPEN...." << std::endl;}


protected:
	StringMap mArguments;
};
typedef boost::shared_ptr<CommandLineStreamer> CommandLineStreamerPtr;
}

#endif /* CXSTREAMER_H_ */
