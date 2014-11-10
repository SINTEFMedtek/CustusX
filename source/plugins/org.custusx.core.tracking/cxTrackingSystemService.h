#ifndef CXTRACKINGSYSTEMSERVICE_H
#define CXTRACKINGSYSTEMSERVICE_H

#include "org_custusx_core_tracking_Export.h"
#include "cxTool.h"

namespace cx
{

typedef boost::shared_ptr<class TrackingSystemService> TrackingSystemServicePtr;
typedef boost::shared_ptr<class TrackerConfiguration> TrackerConfigurationPtr;

/** \brief Tracking System Service
 *
 * Represents one tracking system consisting of a collection of tools.
 * Several such may exist.
 *
 *  \ingroup cx_service_tracking
 *  \date 2014-09-30
 *  \author Christian Askeland, SINTEF
 */
class org_custusx_core_tracking_EXPORT TrackingSystemService : public QObject
{
	Q_OBJECT
public:
	virtual ~TrackingSystemService() {}

//	virtual QString getUid() const = 0;
	virtual Tool::State getState() const = 0;
	virtual void setState(const Tool::State val) = 0; ///< asynchronously request a state. Wait for signal stateChanged()
	virtual std::vector<ToolPtr> getTools() = 0;
	virtual TrackerConfigurationPtr getConfiguration() = 0;
	virtual ToolPtr getReference() = 0; ///< reference tool used by entire tracking service - NOTE: system fails if several TrackingSystemServices define this tool

	virtual void setLoggingFolder(QString loggingFolder) = 0; ///<\param loggingFolder path to the folder where logs should be saved

//	virtual bool isNull() = 0;
//	static TrackingSystemServicePtr getNullObject();
signals:
	void stateChanged();
public slots:
};

}

#endif // CXTRACKINGSYSTEMSERVICE_H
