/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTOOLMETRIC_H_
#define CXTOOLMETRIC_H_

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include "cxFrameMetricBase.h"
#include "cxPointMetric.h"

namespace cx
{

typedef boost::shared_ptr<class ToolMetric> ToolMetricPtr;


/** Metric class containing a snapshot of a tool
 *  at a given point in time.
 *
 *  The transform is attached to a specific coordinate system / frame.
 *
 * \ingroup cx_resource_core_data
 * \date Aug 30, 2013
 * \author Ole Vegard Solberg, SINTEF
 * \author Christian Askeland, SINTEF
 */
class cxResource_EXPORT ToolMetric: public cx::FrameMetricBase
{
Q_OBJECT
public:
	virtual ~ToolMetric();
	static ToolMetricPtr create(QString uid, QString name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider);

	double getToolOffset() const;
	void setToolOffset(double val);
	QString getToolName() const;
	void setToolName(const QString& val);
	virtual QString getType() const
	{
		return getTypeName();
	}
	static QString getTypeName()
	{
		return "ToolMetric";
	}
	virtual QIcon getIcon() {return QIcon(":/icons/metric_tool.png");}
	virtual void addXml(QDomNode& dataNode); ///< adds xml information about the data and its variabels
	virtual void parseXml(QDomNode& dataNode); ///< Use a XML node to load data. \param dataNode A XML data representation of this object.

private:
	ToolMetric(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider);
	double mToolOffset;
	QString mToolName;
};

} //namespace cx

#endif /* CXTOOLMETRIC_H_ */
