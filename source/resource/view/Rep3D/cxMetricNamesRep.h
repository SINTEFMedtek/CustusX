/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXMETRICNAMESREP_H
#define CXMETRICNAMESREP_H

#include "cxResourceVisualizationExport.h"

#include <vector>
#include "cxRepImpl.h"
#include "cxVector3D.h"
#include "vtkForwardDeclarations.h"
#include "cxBoundingBox3D.h"
#include "cxForwardDeclarations.h"
class QColor;

#include "cxRepImpl.h"

namespace cx
{

typedef boost::shared_ptr<class MetricNamesRep> MetricNamesRepPtr;
typedef boost::shared_ptr<class TextDisplay> TextDisplayPtr;
typedef boost::shared_ptr<class DataMetric> DataMetricPtr;

/** 
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep2D
 * \ingroup cx_resource_view_rep3D
 *
 * \date 2014-02-13
 * \author christiana
 */
class cxResourceVisualization_EXPORT MetricNamesRep : public RepImpl
{
	Q_OBJECT
public:
	static MetricNamesRepPtr New(const QString& uid="");
	virtual ~MetricNamesRep();

	virtual QString getType() const{ return "MetricNamesRep";}
	void setFontSize(int size); ///< must be set before setting data
	void setData(std::vector<DataPtr> data);

private slots:
	void callSetColoredTextListSlot();

protected:
	MetricNamesRep();
	virtual void addRepActorsToViewRenderer(ViewPtr view);
	virtual void removeRepActorsFromViewRenderer(ViewPtr view);
	virtual void onModifiedStartRender();

	/** Add a list of colored strings at pos.
	  * This is not directly supported by VTK, so the input viewport is required for extra calculations,
	  * if renderer is set, this is not required.
	  */
	void setColoredTextList(std::vector<std::pair<QColor, QString> > text, Eigen::Array2d pos, vtkViewport *vp=0);

//	vtkRendererPtr getRenderer();
	DoubleBoundingBox3D findNormalizedBoundingBoxAroundText(std::vector<std::pair<QColor, QString> > text, Eigen::Array2d pos, vtkViewport *vp);
	DoubleBoundingBox3D moveBoxIntoAnother(DoubleBoundingBox3D box, DoubleBoundingBox3D another);
	bool equal(std::vector<DataMetricPtr> a, std::vector<DataMetricPtr> b) const;
	std::vector<DataMetricPtr> convertToMetrics(std::vector<DataPtr> data);
	std::vector<std::pair<QColor, QString> > getAllMetricTexts() const;
	QString getText(DataMetricPtr metric, bool showLabel) const;

	int mFontSize;
	std::vector<DataMetricPtr> mMetrics;
	std::vector<TextDisplayPtr> mDisplayText;
};

} // namespace cx

#endif // CXMETRICNAMESREP_H
