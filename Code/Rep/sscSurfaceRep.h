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

#ifndef SSCSURFACEREP_H_
#define SSCSURFACEREP_H_

#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"
#include "sscRepImpl.h"

namespace ssc
{

/**\brief NOT IN USE
 *
 * If you want to use this class, test is thoroughly first!!
 *
 * \ingroup sscRep
 * \ingroup sscNotUsed
 */
class SurfaceRep : public RepImpl
{
	Q_OBJECT
public:
	static SurfaceRepPtr New(const QString& uid);
	virtual QString getType() const { return "ssc::SurfaceRep"; }
	virtual ~SurfaceRep();
	void setMesh(MeshPtr mesh);
	void setImage(ImagePtr image);
	bool hasMesh(MeshPtr mesh) const;

protected:
	SurfaceRep(const QString& uid);
	virtual void addRepActorsToViewRenderer(View *view);
	virtual void removeRepActorsFromViewRenderer(View *view);

	vtkPolyDataMapperPtr mMapper;
	vtkContourFilterPtr mContourFilter;
	vtkPropertyPtr mProperty;
	vtkActorPtr mActor;

	ImagePtr mImage;
	MeshPtr mMesh;
};

//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------

#endif /*SSCSURFACEREP_H_*/

