// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#ifndef SSCPROBE_H_
#define SSCPROBE_H_

#include <QObject>
#include <boost/weak_ptr.hpp>
#include "sscProbeData.h"
#include "sscForwardDeclarations.h"

namespace cx
{
/**
 * \brief US Probe interface.
 *
 * Available from Tool when Tool is a Probe.
 *
 * \ingroup sscTool
 */
class Probe: public QObject
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

	virtual void applyNewConfigurationWithId(QString uid) = 0;
	virtual void setTemporalCalibration(double val) = 0;
	virtual void setSoundSpeedCompensationFactor(double val) = 0;
	virtual void setProbeSector(ProbeData probeSector) = 0;
	virtual void setRTSource(VideoSourcePtr source) = 0;
	virtual void removeRTSource(VideoSourcePtr source) = 0;

	virtual void setActiveStream(QString uid) = 0;
	virtual QString getActiveStream() const = 0;

	virtual QStringList getAvailableVideoSources() = 0; ///< Return a list of all available video source. The default is one with uid=='active'.
	virtual VideoSourcePtr getRTSource(QString uid = "active") const = 0; ///< Return a VideoSource for the given uid. Use 'active' to get the default stream.
	virtual ProbeData getProbeData(QString uid = "active") const = 0; ///< Return a ProbeData for the given uid. Use 'active' to get the default.
	virtual ProbeSectorPtr getSector(QString uid = "active") = 0; /// < Return a ProbeSectorPtr for the given uid. Use 'active' to get the default.

signals:
	void sectorChanged();
	void activeConfigChanged();
};

typedef boost::shared_ptr<Probe> ProbePtr;
typedef boost::weak_ptr<Probe> ProbeWeakPtr;

}//namespace cx

#endif /* SSCPROBE_H_ */
