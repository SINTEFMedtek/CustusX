/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep2D
 */
class cxResourceVisualization_EXPORT CrossHairRep2D : public RepImpl
{
	Q_OBJECT
public:
	static CrossHairRep2DPtr New(SpaceProviderPtr spaceProvider, const QString& uid="");
	virtual ~CrossHairRep2D();
	virtual QString getType() const;

	void setSliceProxy(SliceProxyPtr slicer);
	void set_vpMs(const Transform3D& vpMs);

private slots:
	void sliceTransformChangedSlot(Transform3D sMr); 
	void toolTransformAndTimestampSlot(Transform3D prMt, double timestamp); 
	void toolVisibleSlot(bool visible); 

protected:
	CrossHairRep2D(SpaceProviderPtr spaceProvider);
	virtual void addRepActorsToViewRenderer(ViewPtr view);
	virtual void removeRepActorsFromViewRenderer(ViewPtr view);

private:
	void update();
	CrossHair2DPtr mCursor;
	SliceProxyPtr mSlicer;
	Transform3D m_vpMs;
	SpaceProviderPtr mSpaceProvider;
};


} // namespace vm

#endif /* CXCROSSHAIRREP2D_H_ */
