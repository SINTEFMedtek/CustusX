/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXLOCALSERVERSTREAMERSERVER_H
#define CXLOCALSERVERSTREAMERSERVER_H

#include "org_custusx_core_video_Export.h"
#include "cxStreamerService.h"
#include "cxStreamer.h"
#include "cxProcessWrapper.h"

class ctkPluginContext;

namespace cx
{
typedef boost::shared_ptr<class StringPropertyBase> StringPropertyBasePtr;
typedef boost::shared_ptr<class DoublePropertyBase> DoublePropertyBasePtr;
typedef boost::shared_ptr<class Property> PropertyPtr;
typedef boost::shared_ptr<class FilePathProperty> FilePathPropertyPtr;
typedef boost::shared_ptr<class BoolPropertyBase> BoolPropertyBasePtr;


/** Options for LocalServerStreamer
 *
 * \ingroup org_custusx_core_video
 *
 * \date 2014-11-21
 * \author Christian Askeland, SINTEF
 */
class org_custusx_core_video_EXPORT LocalServerStreamerArguments
{
public:
	std::vector<PropertyPtr> getSettings(QDomElement root);

	BoolPropertyBasePtr getRunLocalServerOption(QDomElement root);
	FilePathPropertyPtr getLocalServerNameOption(QDomElement root);
};

/** Streamer wrapping another Streamer, but also runs an executable as a local process.
 *
 * \ingroup org_custusx_core_video
 *
 * \date 2014-11-21
 * \author Christian Askeland, SINTEF
 */
class org_custusx_core_video_EXPORT LocalServerStreamer: public Streamer
{
Q_OBJECT

public:
	LocalServerStreamer(QString serverName, QString serverArguments);
	virtual ~LocalServerStreamer();
	virtual void startStreaming(SenderPtr sender);
	virtual void stopStreaming();
	virtual bool isStreaming();

	static StreamerPtr createStreamerIfEnabled(QDomElement root, StringMap args);

private slots:
	virtual void streamSlot() {}

	void processStateChanged();
private:
	bool localVideoServerIsRunning();
	ProcessWrapperPtr mLocalVideoServerProcess;

	StreamerPtr mBase;
	QString mServerName;
	QString mServerArguments;
};
typedef boost::shared_ptr<class LocalServerStreamer> LocalServerStreamerPtr;

} //end namespace cx


#endif // CXLOCALSERVERSTREAMERSERVER_H
