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

#ifndef CXRecordSession_H_
#define CXRecordSession_H_

#include <QString>
#include <map>
#include "boost/shared_ptr.hpp"
//#include "cxToolManager.h"
#include "cxForwardDeclarations.h"
#include "cxTransform3D.h"

class QDomNode;

namespace cx
{
/**
 * \file
 * \addtogroup cx_plugin_acquisition
 * @{
 */

typedef boost::shared_ptr<class RecordSession> RecordSessionPtr;
typedef std::map<double, Transform3D> TimedTransformMap;

/**
 * RecordSession
 *
 * \brief
 *
 * \date Dec 8, 2010
 * \author Janne Beate Bakeng
 */
class RecordSession
{
public:
	RecordSession(QString uid, double startTime, double stopTime, QString description);
	virtual ~RecordSession();

	QString getUid();
	QString getDescription();
	double getStartTime();
	double getStopTime();

	void setStopTime(double val) { mStopTime = val; }

	void addXml(QDomNode& dataNode);
	void parseXml(QDomNode& dataNode);

	static TimedTransformMap getToolHistory_prMt(ToolPtr tool, RecordSessionPtr session);

protected:

	QString mUid;
	double mStartTime;
	double mStopTime;
	QString mDescription;
};

/**
 * @}
 */
}//namespace cx

#endif /* CXRecordSession_H_ */
