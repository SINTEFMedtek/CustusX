// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

/*
 * vmCrossHairRep2D.h
 *
 *  Created on: Jan 13, 2009
 *      Author: christiana
 */

#ifndef SSCCROSSHAIRREP2D_H_
#define SSCCROSSHAIRREP2D_H_

//#include <vtkSmartPointer.h>
#include "sscRepImpl.h"
#include "sscTransform3D.h"

namespace ssc
{
typedef boost::shared_ptr<class SliceProxy> SliceProxyPtr;
typedef boost::shared_ptr<class CrossHair2D> CrossHair2DPtr;

typedef boost::shared_ptr<class CrossHairRep2D> CrossHairRep2DPtr;

/**\brief Display the Tool in 2D as a crosshair in the tool tip position.
 *
 * This is a simple Rep that can be used instead of the more elaborate ToolRep2D.
 *
 * \ingroup sscRep
 * \ingroup sscRep2D
 */
class CrossHairRep2D : public ssc::RepImpl
{
	Q_OBJECT
public:
	static CrossHairRep2DPtr New(const QString& uid, const QString& name="");
	virtual ~CrossHairRep2D();
	virtual QString getType() const;

	void setSliceProxy(ssc::SliceProxyPtr slicer);
	void set_vpMs(const Transform3D& vpMs);

private slots:
	void sliceTransformChangedSlot(Transform3D sMr); 
	void toolTransformAndTimestampSlot(Transform3D prMt, double timestamp); 
	void toolVisibleSlot(bool visible); 

protected:
	CrossHairRep2D(const QString& uid, const QString& name="");
	virtual void addRepActorsToViewRenderer(ssc::View *view);
	virtual void removeRepActorsFromViewRenderer(ssc::View *view);

private:
	void update();
	CrossHair2DPtr mCursor;
	SliceProxyPtr mSlicer;
	Transform3D m_vpMs;
};


} // namespace vm

#endif /* SSCCROSSHAIRREP2D_H_ */
