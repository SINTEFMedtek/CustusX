/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXCUSTOMMETRICREP_H
#define CXCUSTOMMETRICREP_H

#include "cxResourceVisualizationExport.h"

#include "cxDataMetricRep.h"
#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include "cxGraphicalObjectWithDirection.h"
#include "cxTransform3D.h"


namespace cx
{
typedef boost::shared_ptr<class CustomMetricRep> CustomMetricRepPtr;
typedef boost::shared_ptr<class CustomMetric> CustomMetricPtr;
typedef boost::shared_ptr<class GraphicalGeometric> GraphicalGeometricPtr;
typedef boost::shared_ptr<class Image2DProxy> Image2DProxyPtr;


/**Rep for visualizing a CustomMetric.
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep3D
 *
 * \date 2016-05-11
 * \author Jon Eiesland, SINTEF
 */
class cxResourceVisualization_EXPORT CustomMetricRep: public DataMetricRep
{
Q_OBJECT
public:
    static CustomMetricRepPtr New(const QString& uid = ""); ///constructor
    virtual ~CustomMetricRep() {}

    virtual QString getType() const { return "CustomMetricRep"; }

protected:
	virtual void clear();
	virtual void onModifiedStartRender();

	virtual void onEveryRender();
private:
    CustomMetricRep();
    CustomMetricPtr getCustomMetric();
	void updateModel();

	std::vector<GraphicalGeometricPtr> mMeshGeometry;
	std::vector<Image2DProxyPtr> mImageGeometryProxy;
	std::vector<CaptionText3DPtr> mDistanceText;
	void updateMeshModel(DataPtr model);
	void updateImageModel(DataPtr model);
	CaptionText3DPtr createDistanceText(Vector3D pos, double distance);
	void createDistanceMarkers();
	void hideDistanceMetrics();
	bool isCloseToCamera(Vector3D pos);
};

}

#endif // CXCUSTOMMETRICREP_H
