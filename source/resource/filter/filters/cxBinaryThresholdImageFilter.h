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

#ifndef CXSBINARYTHRESHOLDIMAGEFILTER_H_
#define CXSBINARYTHRESHOLDIMAGEFILTER_H_

#include "cxFilterImpl.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_resource_filter
 * @{
 */

/** Filter wrapping a itk::BinaryThresholdImageFilter.
 *
 * \ingroup cx_resource_filter
 * \date Nov 21, 2012
 * \author christiana
 */
class cxResourceFilter_EXPORT BinaryThresholdImageFilter : public FilterImpl
{
	Q_OBJECT

public:
	BinaryThresholdImageFilter(VisServicesPtr services);
	virtual ~BinaryThresholdImageFilter() {}

	virtual QString getType() const;
	virtual QString getName() const;
	virtual QString getHelp() const;
	virtual void setActive(bool on);

	bool preProcess();
	virtual bool execute();
	virtual bool postProcess();

	// extensions:
	DoublePairPropertyPtr getThresholdOption(QDomElement root);
	ColorPropertyPtr getColorOption(QDomElement root);
	BoolPropertyPtr getGenerateSurfaceOption(QDomElement root);

protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();

protected slots:
	void thresholdSlot();
private slots:
	/** Set new value+range of the threshold option.
	  */
	void imageChangedSlot(QString uid);

private:
	void stopPreview();

	DoublePairPropertyPtr mThresholdOption;
	vtkImageDataPtr mRawResult;
	vtkPolyDataPtr mRawContour;

protected:
	ImagePtr mPreviewImage;
};
typedef boost::shared_ptr<class BinaryThresholdImageFilter> BinaryThresholdImageFilterPtr;


/**
 * @}
 */
}//namespace cx

#endif /* CXSBINARYTHRESHOLDIMAGEFILTER_H_ */
