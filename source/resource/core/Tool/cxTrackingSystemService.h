#ifndef CXTRACKINGSYSTEMSERVICE_H
#define CXTRACKINGSYSTEMSERVICE_H

#include "cxResourceExport.h"
#include "cxTool.h"

#define TrackingSystemService_iid "cx::TrackingSystemService"

namespace cx
{

typedef boost::shared_ptr<class TrackingSystemService> TrackingSystemServicePtr;
typedef boost::shared_ptr<class TrackerConfiguration> TrackerConfigurationPtr;

/** \brief Tracking System Service
 *
 * Represents one tracking system consisting of a collection of tools.
 * Several such may exist.
 *
 * \ingroup cx_resource_core_tool
 *  \date 2014-09-30
 *  \author Christian Askeland, SINTEF
 */
class cxResource_EXPORT TrackingSystemService : public QObject
{
	Q_OBJECT
public:
	virtual ~TrackingSystemService() {}

	virtual QString getUid() const = 0;
	virtual Tool::State getState() const = 0;
	virtual void setState(const Tool::State val) = 0; ///< asynchronously request a state. Wait for signal stateChanged()
	virtual std::vector<ToolPtr> getTools() = 0;
	virtual TrackerConfigurationPtr getConfiguration() = 0;
	virtual ToolPtr getReference() = 0; ///< reference tool used by entire tracking service - NOTE: system fails if several TrackingSystemServices define this tool

	virtual void setLoggingFolder(QString loggingFolder) = 0; ///<\param loggingFolder path to the folder where logs should be saved

signals:
	void stateChanged();
	void newProbe(const ToolPtr probe);

};
} //namespace cx
Q_DECLARE_INTERFACE(cx::TrackingSystemService, TrackingSystemService_iid)

#endif // CXTRACKINGSYSTEMSERVICE_H
