/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
	Transform3D getMatrixFromSingleLineString(QStringList& parameterList, int indexWhereMatrixStarts);

	CoordinateSystem mSpace;
	SpaceListenerPtr mSpaceListener;
	Transform3D mFrame; ///< frame qFt described in local space q = mSpace

};

} //namespace cx
#endif // CXFRAMEMETRICBASE_H_
