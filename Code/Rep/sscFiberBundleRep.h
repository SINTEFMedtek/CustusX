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

#ifndef SSCFIBERUBUNDLEREP_H_
#define SSCFIBERUBUNDLEREP_H_

#include "vtkForwardDeclarations.h"

#include "sscRepImpl.h"
#include "sscTransform3D.h"
#include "sscForwardDeclarations.h"
#include "sscVtkHelperClasses.h"
#include "sscViewportListener.h"

namespace ssc
{

typedef boost::shared_ptr<class VideoGraphics> VideoSourceGraphicsPtr;
typedef boost::shared_ptr<class ToolTracer> ToolTracerPtr;

/**
 * \brief Display a DTI Fiber bundle (fiber tracks) in 3D.
 *
 * FiberBundleRep displays a DTI fiber bundle using the supplied graphical
 * representation from the FiberBundle interface.
 * Used by Sonowand.
 *
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

} // namespace ssc

#endif /*SSCFIBERUBUNDLEREP_H_*/
