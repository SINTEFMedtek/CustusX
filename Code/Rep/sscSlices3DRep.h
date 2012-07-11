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
 * sscSlices3DRep.h
 *
 *  Created on: Oct 13, 2011
 *      Author: christiana
 */

#ifndef SSCSLICES3DREP_H_
#define SSCSLICES3DREP_H_

#include "sscRepImpl.h"
#include <vector>
#include "sscTransform3D.h"
#include "sscBoundingBox3D.h"
#include "sscDefinitions.h"

#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"

//---------------------------------------------------------
namespace ssc
{

typedef boost::shared_ptr<class Slices3DRep> Slices3DRepPtr;
typedef boost::shared_ptr<class Texture3DSlicerProxy> Texture3DSlicerProxyPtr;

/**\brief Display several slices through volumes in 3D.
 *
 * Use this as an alternative to full volume rendering.
 * Define a set of planes and an ordered set of images.
 * The images are sliced in a way similar to that in
 * Texture3DSlicerRep, but they are displayed in the
 * 3D View instead of in 2D.
 *
 * Used by CustusX.
 *
 * \sa Texture3DSlicerProxy
 *
 * \ingroup sscRep
 * \ingroup sscRep3D
 */
class Slices3DRep: public ssc::RepImpl
{
Q_OBJECT
public:
	static Slices3DRepPtr New(const QString& uid);
	virtual ~Slices3DRep();
	virtual QString getType() const { return "ssc::Slices3DRep"; }

	void setShaderFile(QString shaderFile);
	void setImages(std::vector<ssc::ImagePtr> images);
	void addPlane(PLANE_TYPE plane);
	void setTool(ToolPtr tool);

protected:
	Slices3DRep(const QString& uid);
	virtual void addRepActorsToViewRenderer(ssc::View *view);
	virtual void removeRepActorsFromViewRenderer(ssc::View *view);

private:
	View *mView;
	std::vector<Texture3DSlicerProxyPtr> mProxy;
};
//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------

#endif /* SSCSLICES3DREP_H_ */
