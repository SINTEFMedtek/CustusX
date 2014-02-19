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
class FrameMetricBase: public DataMetric
{
	Q_OBJECT
public:
	FrameMetricBase(const QString& uid, const QString& name = "");
	virtual ~FrameMetricBase();
	virtual QString getType() const = 0;

	virtual void setFrame(const Transform3D& qMt);
	virtual Transform3D getFrame();
	virtual Vector3D getCoordinate() const;
	virtual Vector3D getRefCoord() const; ///< as getRefFrame, but coord only.
	virtual Transform3D getRefFrame() const; ///< return frame described in ref space r : rFt = rMq * qFt
	virtual void setSpace(CoordinateSystem space); // use parentframe from Data
	virtual CoordinateSystem getSpace() const; // use parentframe from Data
	virtual DoubleBoundingBox3D boundingBox() const;

	virtual QString getValueAsString() const { return ""; }
	virtual bool showValueInGraphics() const { return false; }

protected:
	QString matrixAsSingleLineString() const;
	CoordinateSystem mSpace;
	CoordinateSystemListenerPtr mSpaceListener;
	Transform3D mFrame; ///< frame qFt described in local space q = mSpace

};

} //namespace cx
#endif // CXFRAMEMETRICBASE_H_
