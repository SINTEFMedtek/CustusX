/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXFIBERBUNDLEREP_H_
#define CXFIBERBUNDLEREP_H_

#include "cxResourceVisualizationExport.h"

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
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep3D
 */
class cxResourceVisualization_EXPORT FiberBundleRep : public RepImpl
{
	Q_OBJECT

public:
	static FiberBundleRepPtr New(const QString& uid="");
	virtual ~FiberBundleRep() {}

	/** Return rep type description */
	virtual QString getType() const;

	/** Assign a fiber bundle */
	virtual void setBundle(MeshPtr bundle);
	virtual bool hasBundle(MeshPtr bundle) const { return (mBundle == bundle); }
	/** Return currently assigned fiber bundle */
	virtual MeshPtr getBundle() { return mBundle; }

protected:
	FiberBundleRep();
	virtual void addRepActorsToViewRenderer(ViewPtr view);
	virtual void removeRepActorsFromViewRenderer(ViewPtr view);

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
