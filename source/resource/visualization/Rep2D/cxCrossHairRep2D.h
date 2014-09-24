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


/*
 * vmCrossHairRep2D.h
 *
 *  Created on: Jan 13, 2009
 *      Author: christiana
 */

#ifndef CXCROSSHAIRREP2D_H_
#define CXCROSSHAIRREP2D_H_

#include "cxResourceVisualizationExport.h"

//#include <vtkSmartPointer.h>
#include "cxRepImpl.h"
#include "cxTransform3D.h"

namespace cx
{
typedef boost::shared_ptr<class SliceProxy> SliceProxyPtr;
typedef boost::shared_ptr<class CrossHair2D> CrossHair2DPtr;

typedef boost::shared_ptr<class CrossHairRep2D> CrossHairRep2DPtr;
typedef boost::shared_ptr<class SpaceProvider> SpaceProviderPtr;

/**\brief Display the Tool in 2D as a crosshair in the tool tip position.
 *
 * This is a simple Rep that can be used instead of the more elaborate ToolRep2D.
 *
 * \ingroup cx_resource_visualization
 * \ingroup cx_resource_visualization_rep2D
 */
class cxResourceVisualization_EXPORT CrossHairRep2D : public RepImpl
{
	Q_OBJECT
public:
	static CrossHairRep2DPtr New(SpaceProviderPtr spaceProvider, const QString& uid, const QString& name="");
	virtual ~CrossHairRep2D();
	virtual QString getType() const;

	void setSliceProxy(SliceProxyPtr slicer);
	void set_vpMs(const Transform3D& vpMs);

private slots:
	void sliceTransformChangedSlot(Transform3D sMr); 
	void toolTransformAndTimestampSlot(Transform3D prMt, double timestamp); 
	void toolVisibleSlot(bool visible); 

protected:
	CrossHairRep2D(SpaceProviderPtr spaceProvider, const QString& uid, const QString& name="");
	virtual void addRepActorsToViewRenderer(View *view);
	virtual void removeRepActorsFromViewRenderer(View *view);

private:
	void update();
	CrossHair2DPtr mCursor;
	SliceProxyPtr mSlicer;
	Transform3D m_vpMs;
	SpaceProviderPtr mSpaceProvider;
};


} // namespace vm

#endif /* CXCROSSHAIRREP2D_H_ */
