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

#ifndef CXTFRAMEMETRIC_H_
#define CXTFRAMEMETRIC_H_

#include "cxFrameMetricBase.h"
#include "sscDataReaderWriter.h"

namespace cx
{

typedef boost::shared_ptr<class FrameMetric> FrameMetricPtr;

/** \brief ssc::DataReader implementation for FrameMetric
 *
 * \date Aug 16, 2011
 * \author Ole Vegard Solberg, SINTEF
 */
class FrameMetricReader: public ssc::DataReader
{
public:
	virtual ~FrameMetricReader()
	{
	}
	virtual bool canLoad(const QString& type, const QString& filename)
	{
		return type == "frameMetric";
	}
	virtual ssc::DataPtr load(const QString& uid, const QString& filename);
};

/**\brief Data class that represents a single frame (transform).
 *
 * The transform is attached to a specific coordinate system / frame.
 *
 * \date Aug 25, 2013
 * \author Ole Vegard Solberg, SINTEF
 */
class FrameMetric: public cx::FrameMetricBase
{
Q_OBJECT
public:
	FrameMetric(const QString& uid, const QString& name = "");
	virtual ~FrameMetric();
    static FrameMetricPtr create(QDomNode node);
    static FrameMetricPtr create(QString uid, QString name="");
	virtual QString getType() const
	{
		return "frameMetric";
	}
	virtual void addXml(QDomNode& dataNode); ///< adds xml information about the data and its variabels
	virtual void parseXml(QDomNode& dataNode); ///< Use a XML node to load data. \param dataNode A XML data representation of this object.
	virtual QString getAsSingleLineString() const;
};

} //namespace cx

#endif /* CXTFRAMEMETRIC_H_ */
