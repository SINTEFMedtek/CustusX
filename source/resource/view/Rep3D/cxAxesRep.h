/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXAXESREP_H_
#define CXAXESREP_H_

#include "cxResourceVisualizationExport.h"

#include <vector>
#include "cxRepImpl.h"
#include "cxTransform3D.h"
#include "vtkForwardDeclarations.h"

namespace cx
{
typedef boost::shared_ptr<class AxesRep> AxesRepPtr;
typedef boost::shared_ptr<class GraphicalAxes3D> GraphicalAxes3DPtr;
typedef boost::shared_ptr<class ViewportListener> ViewportListenerPtr;

/** \brief Representation for one 3D coordinate axis triplet.
 *
 * Set the position using setTransform().
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep3D
 */
class cxResourceVisualization_EXPORT AxesRep: public RepImpl
{
public:
	virtual ~AxesRep();

	static AxesRepPtr New(const QString& uid="");

	virtual QString getType() const { return "AxesRep"; }
	void setTransform(Transform3D rMt);
	void setFontSize(double size);
	void setAxisLength(double length);
	void setVisible(bool on);
	void setShowAxesLabels(bool on);
	void setCaption(const QString& caption, const Vector3D& color);

protected:
	AxesRep();
	virtual void addRepActorsToViewRenderer(ViewPtr view);
	virtual void removeRepActorsFromViewRenderer(ViewPtr view);
    GraphicalAxes3DPtr mAxes;
};

} // namespace cx

#endif /*CXAXESREP_H_*/
