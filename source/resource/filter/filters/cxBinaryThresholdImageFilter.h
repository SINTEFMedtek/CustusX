/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
