// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXGRAPHICALAXES3D_H
#define CXGRAPHICALAXES3D_H

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
class GraphicalAxes3D
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
