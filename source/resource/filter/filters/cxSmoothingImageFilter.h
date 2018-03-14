/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXSMOOTHINGIMAGEFILTER_H
#define CXSMOOTHINGIMAGEFILTER_H

#include "cxFilterImpl.h"

namespace cx
{

/** Filter for smoothing a volume.
 *
 *
 * \ingroup cx_resource_filter
 * \date Nov 26, 2012
 * \author christiana
 */
class cxResourceFilter_EXPORT SmoothingImageFilter : public FilterImpl
{
	Q_OBJECT

public:
	SmoothingImageFilter(VisServicesPtr services);
	virtual ~SmoothingImageFilter() {}

	virtual QString getType() const;
	virtual QString getName() const;
	virtual QString getHelp() const;

	virtual bool execute();
	virtual bool postProcess();

	// extensions:
	DoublePropertyPtr getSigma(QDomElement root);

protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();

private slots:

private:
	vtkImageDataPtr mRawResult;
};
typedef boost::shared_ptr<class SmoothingImageFilter> SmoothingImageFilterPtr;


} // namespace cx



#endif // CXSMOOTHINGIMAGEFILTER_H
