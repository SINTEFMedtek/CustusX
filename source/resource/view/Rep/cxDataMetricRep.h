/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXDATAMETRICREP_H_
#define CXDATAMETRICREP_H_

#include "cxResourceVisualizationExport.h"

#include "cxRepImpl.h"
#include "cxVector3D.h"
#include "vtkForwardDeclarations.h"

namespace cx
{
typedef boost::shared_ptr<class CaptionText3D> CaptionText3DPtr;
typedef boost::shared_ptr<class DataMetric> DataMetricPtr;

/**
 * \brief Base class for all Data Metric reps.
 *
 * \ingroup cx_resource_view

 * \ingroup cx_resource_view_rep3D
 *
 * Handles common functionality: labels, size.
 *
 * \date Jul 31, 2011
 * \author Christian Askeland, SINTEF
 */
class cxResourceVisualization_EXPORT DataMetricRep: public RepImpl
{
Q_OBJECT
public:
	virtual ~DataMetricRep() {}

	void setGraphicsSize(double size);
	void setLabelSize(double size);
	void setShowLabel(bool on);
	void setShowAnnotation(bool on);

    void setDataMetric(DataMetricPtr value);
    DataMetricPtr getDataMetric();

protected:
	DataMetricRep();

	void addRepActorsToViewRenderer(ViewPtr view);
	void removeRepActorsFromViewRenderer(ViewPtr view);
    void drawText();
    virtual void clear(); // reset all internals
    virtual QString getText();

	double mGraphicsSize;
	bool mShowLabel;
	double mLabelSize;
	bool mShowAnnotation;
	Vector3D getColorAsVector3D() const;

    DataMetricPtr mMetric;

private:
	CaptionText3DPtr mText;
};
typedef boost::shared_ptr<class DataMetricRep> DataMetricRepPtr;

}

#endif /* CXDATAMETRICREP_H_ */
