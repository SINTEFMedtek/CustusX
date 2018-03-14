/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTFRAMEMETRIC_H_
#define CXTFRAMEMETRIC_H_

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include "cxFrameMetricBase.h"

namespace cx
{

typedef boost::shared_ptr<class FrameMetric> FrameMetricPtr;

/** \brief Data class that represents a single frame (transform).
 *
 * The transform is attached to a specific coordinate system / frame.
 *
 * \ingroup cx_resource_core_data
 * \date Aug 25, 2013
 * \author Ole Vegard Solberg, SINTEF
 */
class cxResource_EXPORT FrameMetric: public cx::FrameMetricBase
{
Q_OBJECT
public:
	virtual ~FrameMetric();
	static FrameMetricPtr create(QString uid, QString name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider);
	virtual QString getType() const
	{
		return getTypeName();
	}
	static QString getTypeName()
	{
		return "frameMetric";
	}
	virtual QIcon getIcon() {return QIcon(":/icons/metric_frame.png");}
	virtual void addXml(QDomNode& dataNode); ///< adds xml information about the data and its variabels
	virtual void parseXml(QDomNode& dataNode); ///< Use a XML node to load data. \param dataNode A XML data representation of this object.

private:
	FrameMetric(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider);
};

} //namespace cx

#endif /* CXTFRAMEMETRIC_H_ */
