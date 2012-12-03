/*
 * cxAcquisitionData.h
 *
 *  \date Jun 9, 2011
 *      \author christiana
 */

#ifndef CXACQUISITIONDATA_H_
#define CXACQUISITIONDATA_H_

#include <QObject>
class QDomNode;
#include <vector>
#include "cxRecordSession.h"

namespace ssc
{
typedef boost::shared_ptr<class ReconstructManager> ReconstructManagerPtr;
}

namespace cx
{
/**
* \file
* \addtogroup cxPluginAcquisition
* @{
*/

/**Shared Data and resources for the Acquisition Plugin
 *
 */
class AcquisitionData : public QObject
{
	Q_OBJECT

public:
	AcquisitionData(ssc::ReconstructManagerPtr reconstructer);
	virtual ~AcquisitionData();

	void addRecordSession(RecordSessionPtr session);
	void removeRecordSession(RecordSessionPtr session);
	std::vector<RecordSessionPtr> getRecordSessions();
	RecordSessionPtr getRecordSession(QString uid);
	QString getNewUid();

	ssc::ReconstructManagerPtr getReconstructer() { return mReconstructer; };

	//Interface for saving/loading
	void addXml(QDomNode& dataNode); ///< adds xml information about the StateService and its variabels
	void parseXml(QDomNode& dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of the StateService.

signals:
	void  recordedSessionsChanged();

private:
	std::vector<RecordSessionPtr> mRecordSessions;

	// referenced plugins:
	ssc::ReconstructManagerPtr mReconstructer;
};

typedef boost::shared_ptr<AcquisitionData> AcquisitionDataPtr;

/**
* @}
*/
}

#endif /* CXACQUISITIONDATA_H_ */
