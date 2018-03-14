/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXBINARYTHINNINGIMAGEFILTER3DFILTER_H
#define CXBINARYTHINNINGIMAGEFILTER3DFILTER_H

#include "cxFilterImpl.h"

namespace cx
{

/** Filter implementation of the itk::BinaryThinningImageFilter3D
 *
 *
 * \ingroup cx_resource_filter
 * \date 11 22, 2012
 * \author christiana
 */
class cxResourceFilter_EXPORT BinaryThinningImageFilter3DFilter : public FilterImpl
{
	Q_OBJECT

public:
	BinaryThinningImageFilter3DFilter(VisServicesPtr services);
	virtual ~BinaryThinningImageFilter3DFilter() {}

	virtual QString getType() const;
	virtual QString getName() const;
	virtual QString getHelp() const;

	virtual bool preProcess();
	virtual bool execute();
	virtual bool postProcess();

	ColorPropertyBasePtr getColorOption(QDomElement root);

protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();

private:
	vtkImageDataPtr mRawResult;
};


} // namespace cx

#endif // CXBINARYTHINNINGIMAGEFILTER3DFILTER_H
