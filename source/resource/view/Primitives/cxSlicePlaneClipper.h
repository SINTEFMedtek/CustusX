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


#ifndef CXSLICEPLANECLIPPER_H_
#define CXSLICEPLANECLIPPER_H_

#include "cxResourceVisualizationExport.h"

#include <set>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <vtkSmartPointer.h>
#include <QObject>
#include "cxForwardDeclarations.h"
#include "cxVector3D.h"
#include "vtkForwardDeclarations.h"

namespace cx
{
typedef boost::shared_ptr<class SlicePlaneClipper> SlicePlaneClipperPtr;

/*
 * \date Aug 20, 2010
 * \author christiana
 */

/** \brief Clip several 3D volumes using a SliceProxy.
 *
 * Several 3D volumes, represented as VolumetricBaseRep, are decorated with
 * a clip plane defined by a SliceProxy.
 *
 * \ingroup cx_resource_visualization

 */
class cxResourceVisualization_EXPORT SlicePlaneClipper : public QObject
{
	Q_OBJECT
public:
	typedef std::set<VolumetricBaseRepPtr> VolumesType;

	static SlicePlaneClipperPtr New();
	~SlicePlaneClipper();
	void setSlicer(SliceProxyPtr slicer);
	SliceProxyPtr getSlicer();
	void setInvertPlane(bool on);
	bool getInvertPlane() const;
	vtkPlanePtr getClipPlaneCopy();
	vtkPlanePtr getClipPlane();

signals:
	void slicePlaneChanged();
private slots:
	void changedSlot();
private:
	SlicePlaneClipper();
	void updateClipPlane();
	Vector3D getUnitNormal() const;
	SliceProxyPtr mSlicer;
	bool mInvertPlane;

	vtkPlanePtr mClipPlane;
};

} // namespace cx

#endif /* CXSLICEPLANECLIPPER_H_ */
