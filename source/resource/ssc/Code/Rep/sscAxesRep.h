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

#ifndef SSCAXESREP_H_
#define SSCAXESREP_H_

#include <vector>
#include "sscRepImpl.h"
#include "sscTransform3D.h"
#include "vtkForwardDeclarations.h"

namespace ssc
{
typedef boost::shared_ptr<class AxesRep> AxesRepPtr;
typedef boost::shared_ptr<class ViewportListener> ViewportListenerPtr;

/**\brief Representation for one 3D coordinate axis triplet.
 *
 * Set the position using setTransform().
 *
 * \ingroup sscRep
 * \ingroup sscRep3D
 */
class AxesRep: public RepImpl
{
public:
	virtual ~AxesRep();

	static AxesRepPtr New(const QString& uid);

	virtual QString getType() const { return "ssc::AxesRep"; }
	void setTransform(Transform3D rMt);
	void setFontSize(double size);
	void setAxisLength(double length);
	void setVisible(bool on);

	void setShowAxesLabels(bool on);
	void setCaption(const QString& caption, const Vector3D& color);

protected:
	AxesRep(const QString& uid);
	virtual void addRepActorsToViewRenderer(View *view);
	virtual void removeRepActorsFromViewRenderer(View *view);
	void addCaption(const QString& label, Vector3D pos, Vector3D color);
	void rescale();
	vtkAxesActorPtr mActor;
	vtkAssemblyPtr mAssembly;
	std::vector<vtkCaptionActor2DPtr> mCaption;
	std::vector<Vector3D> mCaptionPos;
	double mSize;
	double mFontSize;
	ViewportListenerPtr mViewportListener;
};

} // namespace ssc

#endif /*SSCAXESREP_H_*/
