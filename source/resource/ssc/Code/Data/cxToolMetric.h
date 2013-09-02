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

#include "sscPointMetric.h"
#include "sscCoordinateSystemHelpers.h"
#include "sscDataReaderWriter.h"

namespace cx
{

typedef boost::shared_ptr<class ToolMetric> ToolMetricPtr;

/** \brief ssc::DataReader implementation for ToolMetric
 *
 * \date Aug 16, 2011
 * \author Ole Vegard Solberg, SINTEF
 */
class ToolMetricReader: public ssc::DataReader
{
public:
	virtual ~ToolMetricReader()
	{
	}
	virtual bool canLoad(const QString& type, const QString& filename)
	{
		return type == "ToolMetric";
	}
	virtual ssc::DataPtr load(const QString& uid, const QString& filename);
};

/**\brief Data class that represents a single transform.
 *
 * The transform is attached to a specific coordinate system / frame.
 *
 * \date Aug 30, 2013
 * \author Ole Vegard Solberg, SINTEF
 * \author Christian Askeland, SINTEF
 */
class ToolMetric: public ssc::DataMetric
{
Q_OBJECT
public:
	ToolMetric(const QString& uid, const QString& name = "");
	virtual ~ToolMetric();
	static ToolMetricPtr create(QDomNode node);
	static ToolMetricPtr create(QString uid, QString name="");

	void setFrame(const ssc::Transform3D& qMt);
	ssc::Transform3D getFrame();
	double getToolOffset() const;
	void setToolOffset(double val);
	QString getToolName() const;
	void setToolName(const QString& val);
	ssc::Vector3D getCoordinate() const;
	ssc::Vector3D getRefCoord() const; ///< as getRefFrame, but coord only.
	ssc::Transform3D getRefFrame() const; ///< return frame described in ref space r : rFt = rMq * qFt
	void setSpace(ssc::CoordinateSystem space); // use parentframe from ssc::Data
	ssc::CoordinateSystem getSpace() const; // use parentframe from ssc::Data
	virtual QString getType() const
	{
		return "ToolMetric";
	}
	virtual QString getAsSingleLineString() const;

	virtual void addXml(QDomNode& dataNode); ///< adds xml information about the data and its variabels
	virtual void parseXml(QDomNode& dataNode); ///< Use a XML node to load data. \param dataNode A XML data representation of this object.
	virtual ssc::DoubleBoundingBox3D boundingBox() const;
	QString getAsSingleLineString();

private:
	QString matrixAsSingleLineString() const;
	ssc::CoordinateSystem mSpace;
	ssc::CoordinateSystemListenerPtr mSpaceListener;
	ssc::Transform3D mFrame; ///< frame qFt described in local space q = mSpace
	double mToolOffset;
	QString mToolName;
};

} //namespace cx

#endif /* CXTOOLMETRIC_H_ */
