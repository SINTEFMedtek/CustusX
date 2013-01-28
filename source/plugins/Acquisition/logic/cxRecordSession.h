#ifndef CXRecordSession_H_
#define CXRecordSession_H_

#include <QString>
#include "boost/shared_ptr.hpp"
//#include "cxToolManager.h"

class QDomNode;

namespace cx
{
/**
 * \file
 * \addtogroup cxPluginAcquisition
 * @{
 */

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

protected:

	QString mUid;
	double mStartTime;
	double mStopTime;
	QString mDescription;
};

typedef boost::shared_ptr<RecordSession> RecordSessionPtr;

/**
 * @}
 */
}//namespace cx

#endif /* CXRecordSession_H_ */
