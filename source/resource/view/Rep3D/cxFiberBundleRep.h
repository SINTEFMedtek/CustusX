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
