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
#ifndef CXSMOOTHINGIMAGEFILTER_H
#define CXSMOOTHINGIMAGEFILTER_H

#include "cxFilterImpl.h"

#include "sscDoubleDataAdapterXml.h"

namespace cx
{

/** Filter for smoothing a volume.
 *
 *
 * \ingroup cxPluginAlgorithm
 * \date Nov 26, 2012
 * \author christiana
 */
class SmoothingImageFilter : public FilterImpl
{
	Q_OBJECT

public:
	virtual ~SmoothingImageFilter() {}

	virtual QString getType() const;
	virtual QString getName() const;
	virtual QString getHelp() const;

	virtual bool execute();
	virtual void postProcess();

	// extensions:
	ssc::DoubleDataAdapterXmlPtr getSigma(QDomElement root);

protected:
	virtual void createOptions(QDomElement root);
	virtual void createInputTypes();
	virtual void createOutputTypes();

private slots:

private:
	vtkImageDataPtr mRawResult;
};
typedef boost::shared_ptr<class SmoothingImageFilter> SmoothingImageFilterPtr;


} // namespace cx



#endif // CXSMOOTHINGIMAGEFILTER_H
