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

#ifndef CXMETRICNAMESREP_H
#define CXMETRICNAMESREP_H

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
 * \ingroup cx_resource_visualization
 * \ingroup cx_resource_visualization_rep2D
 * \ingroup cx_resource_visualization_rep3D
 *
 * \date 2014-02-13
 * \author christiana
 */
class MetricNamesRep : public RepImpl
{
	Q_OBJECT
public:
	static MetricNamesRepPtr New(const QString& uid, const QString& name);
	virtual ~MetricNamesRep();

	virtual QString getType() const{ return "MetricNamesRep";}
	void setFontSize(int size); ///< must be set before setting data
	void setData(std::vector<DataPtr> data);


protected:
	MetricNamesRep(const QString& uid, const QString& name);
	virtual void addRepActorsToViewRenderer(View *view);
	virtual void removeRepActorsFromViewRenderer(View *view);
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
