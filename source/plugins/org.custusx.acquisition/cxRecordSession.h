/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXRecordSession_H_
#define CXRecordSession_H_

#include "org_custusx_acquisition_Export.h"

#include <QString>
#include <map>
#include <QDateTime>
#include "boost/shared_ptr.hpp"
#include "cxForwardDeclarations.h"
#include "cxTransform3D.h"
#include "cxTool.h"

class QDomNode;

namespace cx
{
/**
 * \file
 * \addtogroup org_custusx_acquisition
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
class org_custusx_acquisition_EXPORT RecordSession
{
public:
	RecordSession();
	RecordSession(int id, QString category);
	virtual ~RecordSession();

	QString getUid() const;
	QString getHumanDescription() const; ///< description useful for display in gui.
	QString getDescription() const; ///< another legacy uid, used for folder creation etc
	std::pair<QDateTime,QDateTime> getInterval(int i);
	unsigned getIntervalCount() const;

	void startNewInterval();
	void stopLastInterval();
	void cancelLastInterval();

	void addXml(QDomNode& node);
	void parseXml(QDomNode& node);

	static TimedTransformMap getToolHistory_prMt(ToolPtr tool, RecordSessionPtr session, bool verbose);
	static std::map<double, ToolPositionMetadata> getToolHistory_metadata(ToolPtr tool, RecordSessionPtr session, bool verbose);

protected:
	QDateTime getTimestamp() const;

	typedef std::pair<QDateTime, QDateTime> IntervalType;
	std::vector<IntervalType> mIntervals;
	QDateTime mTimestamp;
	QString mCategory;
	int mId;

	QDateTime timestamp2datetime(QString in) const;
	QString datetime2timestamp(QDateTime in) const;

	bool isOldStyleXmlFormat(QDomNode& node);
	void parseXml_oldstyle(QDomNode& parentNode);
	void setIdAndTimestampFromUid(QString uid);
};

/**
 * @}
 */
}//namespace cx

#endif /* CXRecordSession_H_ */
