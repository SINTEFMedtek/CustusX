/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep3D
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
