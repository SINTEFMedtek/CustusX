/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXISLANDSFILTER_H
#define CXISLANDSFILTER_H

#include <random>
#include "cxFilterImpl.h"

namespace cx
{

/** Devide binary volume into islands (not connected areas).
 * Input: Binary volume
 * Output: Labeled volume, separate label for each lsland
 *
 * \ingroup cxResourceAlgorithms
 * \date March 09, 2023
 * \author Erlend F. Hofstad
 */
class cxResourceFilter_EXPORT IslandsFilter : public FilterImpl
{
	Q_OBJECT

public:
	IslandsFilter(VisServicesPtr services);
	virtual ~IslandsFilter() {}

	virtual QString getType() const;
	virtual QString getName() const;
	virtual QString getHelp() const;
	static QString getNameSuffixIslands();
	ImagePtr getOutputImage();

	virtual bool execute();
	ImagePtr execute(ImagePtr inputImage, int minimumSize);
	virtual bool postProcess();

protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();

private:
	DoublePropertyPtr getMinimumSizeOption(QDomElement root);

	ImagePtr mOutputImage;
};

typedef boost::shared_ptr<IslandsFilter> IslandsFilterPtr;

} // namespace cx


#endif // CXISLANDSFILTER_H
