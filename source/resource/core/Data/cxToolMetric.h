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

#ifndef CXTOOLMETRIC_H_
#define CXTOOLMETRIC_H_

#include "cxFrameMetricBase.h"
#include "sscPointMetric.h"
#include "sscDataReaderWriter.h"

namespace cx
{

typedef boost::shared_ptr<class ToolMetric> ToolMetricPtr;

/** \brief DataReader implementation for ToolMetric
 *
 * \date Aug 16, 2011
 * \author Ole Vegard Solberg, SINTEF
 */
class ToolMetricReader: public DataReader
{
public:
	virtual ~ToolMetricReader()
	{
	}
	virtual bool canLoad(const QString& type, const QString& filename)
	{
		return type == "ToolMetric";
	}
	virtual DataPtr load(const QString& uid, const QString& filename);
};

/** Metric class containing a snapshot of a tool
 *  at a given point in time.
 *
 *  The transform is attached to a specific coordinate system / frame.
 *
 * \date Aug 30, 2013
 * \author Ole Vegard Solberg, SINTEF
 * \author Christian Askeland, SINTEF
 */
class ToolMetric: public cx::FrameMetricBase
{
Q_OBJECT
public:
	ToolMetric(const QString& uid, const QString& name = "");
	virtual ~ToolMetric();
	static ToolMetricPtr create(QDomNode node);
	static ToolMetricPtr create(QString uid, QString name="");

	double getToolOffset() const;
	void setToolOffset(double val);
	QString getToolName() const;
	void setToolName(const QString& val);
	virtual QString getType() const
	{
		return "ToolMetric";
	}
	virtual void addXml(QDomNode& dataNode); ///< adds xml information about the data and its variabels
	virtual void parseXml(QDomNode& dataNode); ///< Use a XML node to load data. \param dataNode A XML data representation of this object.
	virtual QString getAsSingleLineString() const;

private:
	double mToolOffset;
	QString mToolName;
};

} //namespace cx

#endif /* CXTOOLMETRIC_H_ */
