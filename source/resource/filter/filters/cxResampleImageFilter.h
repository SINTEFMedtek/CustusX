/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXRESAMPLEIMAGEFILTER_H
#define CXRESAMPLEIMAGEFILTER_H

#include "cxFilterImpl.h"

namespace cx
{

/** Filter for resampling and cropping a volume into the space of another.
 *
 *
 * \ingroup cx_resource_filter
 * \date Nov 26, 2012
 * \author christiana
 */
class cxResourceFilter_EXPORT ResampleImageFilter : public FilterImpl
{
	Q_OBJECT

public:
	ResampleImageFilter(VisServicesPtr services);
	virtual ~ResampleImageFilter() {}

	virtual QString getType() const;
	virtual QString getName() const;
	virtual QString getHelp() const;
	virtual bool execute();
	virtual bool postProcess();

	// extensions:
	DoublePropertyPtr getMarginOption(QDomElement root);

protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();

private slots:

private:
	ImagePtr mRawResult;
};
typedef boost::shared_ptr<class ResampleImageFilter> ResampleImageFilterPtr;


} // namespace cx



#endif // CXRESAMPLEIMAGEFILTER_H
