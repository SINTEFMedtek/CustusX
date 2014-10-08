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

#ifndef CXGRAPHICALAXES3D_H
#define CXGRAPHICALAXES3D_H

#include "cxResourceVisualizationExport.h"

#include <vector>
#include "cxRepImpl.h"
#include "cxTransform3D.h"
#include "vtkForwardDeclarations.h"

namespace cx
{
typedef boost::shared_ptr<class GraphicalAxes3D> GraphicalAxes3DPtr;
typedef boost::shared_ptr<class ViewportListener> ViewportListenerPtr;
}

namespace cx
{

/** \brief Visualization for one 3D coordinate axis triplet.
 *
 * Set the position using setTransform().
 *
 * \ingroup cx_resource_visualization
 * \ingroup cx_resource_visualization_rep3D
 */
class cxResourceVisualization_EXPORT GraphicalAxes3D
{
public:
    GraphicalAxes3D(vtkRendererPtr renderer = vtkRendererPtr());
    void setRenderer(vtkRendererPtr renderer = vtkRendererPtr());
    virtual ~GraphicalAxes3D();

    void setTransform(Transform3D rMt);
    void setFontSize(double size);
    void setAxisLength(double length);
    void setVisible(bool on);

    void setShowAxesLabels(bool on);
    void setCaption(const QString& caption, const Vector3D& color);

protected:
    void addCaption(const QString& label, Vector3D pos, Vector3D color);
    void rescale();
	void resetAxesLabels();
	vtkAxesActorPtr mActor;
    std::vector<vtkCaptionActor2DPtr> mCaption;
    std::vector<Vector3D> mCaptionPos;
    double mSize;
    double mFontSize;
	bool mShowLabels;
    ViewportListenerPtr mViewportListener;
    vtkRendererPtr mRenderer;
	const double m_vtkAxisLength;

	Transform3D m_rMt;
};

} // namespace cx

#endif // CXGRAPHICALAXES3D_H
