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

#ifndef CXFRAMEMETRICBASE_H_
#define CXFRAMEMETRICBASE_H_

#include "sscDataMetric.h"
#include "sscCoordinateSystemListener.h"

namespace cx {

/*
 * \brief Abstract base class for frame metric.
 *
 * \date Sep 2, 2013
 * \author Ole Vegard Solberg, SINTEF
 */
class FrameMetricBase: public ssc::DataMetric
{
	Q_OBJECT
public:
	FrameMetricBase(const QString& uid, const QString& name = "");
	virtual ~FrameMetricBase();
	virtual QString getType() const = 0;

	virtual void setFrame(const ssc::Transform3D& qMt);
	virtual ssc::Transform3D getFrame();
	virtual ssc::Vector3D getCoordinate() const;
	virtual ssc::Vector3D getRefCoord() const; ///< as getRefFrame, but coord only.
	virtual ssc::Transform3D getRefFrame() const; ///< return frame described in ref space r : rFt = rMq * qFt
	virtual void setSpace(ssc::CoordinateSystem space); // use parentframe from ssc::Data
	virtual ssc::CoordinateSystem getSpace() const; // use parentframe from ssc::Data
	virtual ssc::DoubleBoundingBox3D boundingBox() const;

protected:
	QString matrixAsSingleLineString() const;
	ssc::CoordinateSystem mSpace;
	ssc::CoordinateSystemListenerPtr mSpaceListener;
	ssc::Transform3D mFrame; ///< frame qFt described in local space q = mSpace

};

} //namespace cx
#endif // CXFRAMEMETRICBASE_H_
