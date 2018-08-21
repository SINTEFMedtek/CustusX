/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

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
class cxResourceFilter_EXPORT ContourFilter : public FilterImpl
{
	Q_OBJECT

public:
	ContourFilter(VisServicesPtr services);
	virtual ~ContourFilter() {}

	virtual QString getType() const;
	virtual QString getName() const;
	virtual QString getHelp() const;
    static QString getNameSuffix();
	virtual void setActive(bool on);

	bool preProcess();
	virtual bool execute();
	virtual bool postProcess();

	// extensions:
	BoolPropertyPtr getReduceResolutionOption(QDomElement root);
	BoolPropertyPtr getSmoothingOption(QDomElement root);
	BoolPropertyPtr getPreserveTopologyOption(QDomElement root);
	DoublePropertyPtr getSurfaceThresholdOption(QDomElement root);
	DoublePropertyPtr getDecimationOption(QDomElement root);
	ColorPropertyPtr getColorOption(QDomElement root);
    DoublePropertyPtr getNumberOfIterationsOption(QDomElement root);
    DoublePropertyPtr getPassBandOption(QDomElement root);

	/** This is the core algorithm, call this if you dont need all the filter stuff.
	    Generate a contour from a vtkImageData.
	  */
	static vtkPolyDataPtr execute(vtkImageDataPtr input,
			                              double threshold,
	                                      bool reduceResolution=false,
	                                      bool smoothing=true,
	                                      bool preserveTopology=true,
                                          double decimation=0.2,
                                          double numberOfIterations = 15,
                                          double passBand = 0.3);
	/** Generate a mesh from the contour using base to generate name.
	  * Save to dataManager.
	  */
	static MeshPtr postProcess(PatientModelServicePtr patient, vtkPolyDataPtr contour, ImagePtr base, QColor color);

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
	void stopPreview();

	BoolPropertyPtr mReduceResolutionOption;
	DoublePropertyPtr mSurfaceThresholdOption;
	vtkPolyDataPtr mRawResult;
	ImagePtr mPreviewImage;
};
typedef boost::shared_ptr<class ContourFilter> ContourFilterPtr;


} // namespace cx

#endif // CXCONTOURFILTER_H
