/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXFRAMEMETRICBASE_H_
#define CXFRAMEMETRICBASE_H_

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include "cxDataMetric.h"
#include "cxCoordinateSystemHelpers.h"

namespace cx {

/**
 * \brief Abstract base class for frame metric.
 *
 * \ingroup cx_resource_core_data
 * \date Sep 2, 2013
 * \author Ole Vegard Solberg, SINTEF
 */
class cxResource_EXPORT FrameMetricBase: public DataMetric
{
	Q_OBJECT
public:
	FrameMetricBase(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider);
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

	virtual QString getParentSpace();
protected:
	QString matrixAsSingleLineString() const;
	CoordinateSystem mSpace;
	SpaceListenerPtr mSpaceListener;
	Transform3D mFrame; ///< frame qFt described in local space q = mSpace

};

} //namespace cx
#endif // CXFRAMEMETRICBASE_H_
