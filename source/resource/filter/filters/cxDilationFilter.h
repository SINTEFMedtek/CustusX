/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CX_DILATION_FILTER_H
#define CX_DILATION_FILTER_H

#include "cxFilterImpl.h"

namespace cx {
class cxResourceFilter_EXPORT DilationFilter : public FilterImpl
{
	Q_OBJECT

public:
	DilationFilter(VisServicesPtr services);
	virtual ~DilationFilter() {}

	virtual QString getType() const;
	virtual QString getName() const;
	virtual QString getHelp() const;

	bool preProcess();
	virtual bool execute();
	virtual bool postProcess();

	// extensions:
	DoublePropertyPtr getDilationRadiusOption(QDomElement root);
	ColorPropertyPtr getColorOption(QDomElement root);
	BoolPropertyPtr getGenerateSurfaceOption(QDomElement root);

protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();

private:
	DoublePropertyPtr mDilationRadiusOption;
	vtkImageDataPtr mRawResult;
	vtkPolyDataPtr mRawContour;
};
typedef boost::shared_ptr<class DilationFilter> DilationFilterPtr;

} // namespace cx

#endif
