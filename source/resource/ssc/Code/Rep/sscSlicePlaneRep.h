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

#ifndef SSCSLICEPLANEREP_H_
#define SSCSLICEPLANEREP_H_

#include "sscRepImpl.h"
#include <vector>
#include "sscTransform3D.h"
#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"

namespace ssc
{

/**\brief A slicer implementation.
 *        NOT IN USE.
 *
 * Unknown functionality.
 * 
 * \ingroup sscRep
 * \ingroup sscNotUsed
 */
class SlicePlaneRep : public RepImpl
{
	Q_OBJECT

public:
	virtual ~SlicePlaneRep();

	static SlicePlaneRepPtr New(const QString& uid);

	virtual QString getType() const { return "ssc::SliceRep"; }
	void setImage(ImagePtr image);
	bool hasImage(ImagePtr image) const;
	void setSliceProxy(SliceProxyPtr proxy);
	void setSliceReps( std::vector<ssc::SliceRepSWPtr> slicerReps);

protected:
	SlicePlaneRep(const QString& uid);
	virtual void addRepActorsToViewRenderer(View *view);
	virtual void removeRepActorsFromViewRenderer(View *view);

private slots:
	void sliceTransformChangedSlot(Transform3D sMr);

private:
	SliceProxyPtr mSlicer;
	std::vector<SliceRepSWPtr> mSlicesPlanes;
	vtkImagePlaneWidgetPtr mVtkImagePlaneWidgetA;
	vtkImagePlaneWidgetPtr mVtkImagePlaneWidgetC;
	vtkImagePlaneWidgetPtr mVtkImagePlaneWidgetS;

	ImagePtr mImage;
};

} // namespace ssc


#endif /*SSCSLICEPLANEREP_H_*/
