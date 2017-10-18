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
