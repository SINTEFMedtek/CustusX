/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXPROBE_H_
#define CXPROBE_H_

#include "cxResourceExport.h"

#include <QObject>
#include <boost/weak_ptr.hpp>
#include "cxProbeDefinition.h"
#include "cxForwardDeclarations.h"

namespace cx
{
/**
 * \brief US Probe interface.
 *
 * Available from Tool when Tool is a Probe.
 *
 * \ingroup cx_resource_core_tool
 */
class cxResource_EXPORT Probe: public QObject
{
	Q_OBJECT
public:
	virtual ~Probe()
	{
	}
	virtual bool isValid() const = 0;
	virtual void addXml(QDomNode& dataNode) = 0;
	virtual void parseXml(QDomNode& dataNode) = 0;

	virtual QStringList getConfigIdList() const = 0;
	virtual QString getConfigName(QString uid) = 0;
	virtual QString getConfigId() const = 0;
	virtual QString getConfigurationPath() const = 0;

	virtual void removeCurrentConfig() = 0; ///< remove the current config from disk
	virtual void saveCurrentConfig(QString uid, QString name) = 0; ///< save current config to disk under ids (uid,name).

	virtual void applyNewConfigurationWithId(QString uid) = 0;
	virtual void setTemporalCalibration(double val) = 0;
	virtual void setSoundSpeedCompensationFactor(double val) = 0;
	virtual void setProbeDefinition(ProbeDefinition probeDefinition) = 0;
	virtual void setRTSource(VideoSourcePtr source) = 0;
	virtual void removeRTSource(VideoSourcePtr source) = 0;

	virtual void setActiveStream(QString uid) = 0;
	virtual QString getActiveStream() const = 0;

	virtual QStringList getAvailableVideoSources() = 0; ///< Return a list of all available video source. The default is one with uid=='active'.
	virtual VideoSourcePtr getRTSource(QString uid = "active") const = 0; ///< Return a VideoSource for the given uid. Use 'active' to get the default stream.
	virtual ProbeDefinition getProbeDefinition(QString uid = "active") const = 0; ///< Return a ProbeDefinition for the given uid. Use 'active' to get the default.
	virtual ProbeSectorPtr getSector(QString uid = "active") = 0; /// < Return a ProbeSectorPtr for the given uid. Use 'active' to get the default.

signals:
	void sectorChanged();
	void activeConfigChanged();
	void videoSourceAdded(VideoSourcePtr source);
};

typedef boost::shared_ptr<Probe> ProbePtr;
typedef boost::weak_ptr<Probe> ProbeWeakPtr;

}//namespace cx

#endif /* CXPROBE_H_ */
