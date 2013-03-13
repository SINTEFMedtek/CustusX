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

#ifndef CXCONTOURFILTER_H
#define CXCONTOURFILTER_H

#include "cxFilterImpl.h"
class QColor;

namespace cx
{

/** Marching cubes surface generation.
 *
 *
 * \ingroup cx
 * \date Nov 25, 2012
 * \author christiana
 */
class ContourFilter : public FilterImpl
{
	Q_OBJECT

public:
	virtual ~ContourFilter() {}

	virtual QString getType() const;
	virtual QString getName() const;
	virtual QString getHelp() const;
	virtual void setActive(bool on);

	bool preProcess();
	virtual bool execute();
	virtual bool postProcess();

	// extensions:
	ssc::BoolDataAdapterXmlPtr getReduceResolutionOption(QDomElement root);
	ssc::BoolDataAdapterXmlPtr getSmoothingOption(QDomElement root);
	ssc::BoolDataAdapterXmlPtr getPreserveTopologyOption(QDomElement root);
	ssc::DoubleDataAdapterXmlPtr getSurfaceThresholdOption(QDomElement root);
	ssc::DoubleDataAdapterXmlPtr getDecimationOption(QDomElement root);
	ssc::ColorDataAdapterXmlPtr getColorOption(QDomElement root);

	/** This is the core algorithm, call this if you dont need all the filter stuff.
	    Generate a contour from a vtkImageData.
	  */
	static vtkPolyDataPtr execute(vtkImageDataPtr input,
			                              double threshold,
	                                      bool reduceResolution=false,
	                                      bool smoothing=true,
	                                      bool preserveTopology=true,
	                                      double decimation=0.2);
	/** Generate a mesh from the contour using base to generate name.
	  * Save to dataManager.
	  */
	static ssc::MeshPtr postProcess(vtkPolyDataPtr contour, ssc::ImagePtr base, QColor color);

protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();

private slots:
	/** Set new value+range of the threshold option.
	  */
	void imageChangedSlot(QString uid);
	void thresholdSlot();

private:
	ssc::BoolDataAdapterXmlPtr mReduceResolutionOption;
	ssc::DoubleDataAdapterXmlPtr mSurfaceThresholdOption;
	vtkPolyDataPtr mRawResult;
};
typedef boost::shared_ptr<class ContourFilter> ContourFilterPtr;


} // namespace cx

#endif // CXCONTOURFILTER_H
