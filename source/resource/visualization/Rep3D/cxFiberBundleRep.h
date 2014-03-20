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

#ifndef CXFIBERBUNDLEREP_H_
#define CXFIBERBUNDLEREP_H_

#include "vtkForwardDeclarations.h"

#include "cxRepImpl.h"
#include "cxTransform3D.h"
#include "cxForwardDeclarations.h"
#include "cxVtkHelperClasses.h"
#include "cxViewportListener.h"

namespace cx
{

/**
 * \brief Display a DTI Fiber bundle (fiber tracks) in 3D.
 *
 * FiberBundleRep displays a DTI fiber bundle using the supplied graphical
 * representation from the FiberBundle interface.
 * Used by Sonowand.
 *
 * \ingroup cx_resource_visualization
 * \ingroup cx_resource_visualization_rep3D
 */
class FiberBundleRep : public RepImpl
{
	Q_OBJECT

public:
	static FiberBundleRepPtr New(const QString& uid, const QString& name = "");
	virtual ~FiberBundleRep() {}

	/** Return rep type description */
	virtual QString getType() const;

	/** Assign a fiber bundle */
	virtual void setBundle(MeshPtr bundle);
	virtual bool hasBundle(MeshPtr bundle) const { return (mBundle == bundle); }
	/** Return currently assigned fiber bundle */
	virtual MeshPtr getBundle() { return mBundle; }

protected:
	FiberBundleRep(const QString& uid, const QString& name = "");
	virtual void addRepActorsToViewRenderer(View *view);
	virtual void removeRepActorsFromViewRenderer(View *view);

private:
	double  mFiberWidth;

	MeshPtr mBundle;
	ViewportListenerPtr mViewportListener;

	vtkActorPtr mActor;
	vtkPolyDataMapperPtr mPolyDataMapper;
	vtkPropertyPtr mProperty;

	private slots:
	void bundleChanged();
	void bundleTransformChanged();

};

} // namespace cx

#endif /*CXFIBERBUNDLEREP_H_*/
