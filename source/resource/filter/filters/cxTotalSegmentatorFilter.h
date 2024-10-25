/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTOTALSEGMENTATORFILTER_H
#define CXTOTALSEGMENTATORFILTER_H

#include <random>
#include "cxFilterImpl.h"

namespace cx
{

/** Filter to segment structures from CT or MR. Running Total Segmentator: https://github.com/wasserth/TotalSegmentator
 * Input: CT or MR volume
 * Options: Segmented structures to save as mesh and/or volume
 * Output: Segmented structures (mesh and/or volume)
 *
 * \ingroup cxResourceAlgorithms
 * \date Oct 15, 2024
 * \author Erlend F. Hofstad
 */
class cxResourceFilter_EXPORT TotalSegmentatorFilter : public FilterImpl
{
	Q_OBJECT

public:
	TotalSegmentatorFilter(VisServicesPtr services);
	virtual ~TotalSegmentatorFilter() {}

	virtual QString getType() const;
	virtual QString getName() const;
	virtual QString getHelp() const;
	static QString getNameSuffixTotalSegmentator();
	ImagePtr getOutputImage(); //TO DO: Add type or name string
	ImagePtr getOutputMesh(); //TO DO: Add type or name string

	virtual bool execute();
	ImagePtr execute(ImagePtr inputImage);
	virtual bool postProcess();

protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();

private:
	QString getFilterScriptsPath();

};

typedef boost::shared_ptr<TotalSegmentatorFilter> TotalSegmentatorFilterPtr;

} // namespace cx


#endif // CXTOTALSEGMENTATORFILTER_H
