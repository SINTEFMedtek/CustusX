/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXFILTERTIMEDALGORITHM_H
#define CXFILTERTIMEDALGORITHM_H

#include "cxResourceFilterExport.h"

#include "cxThreadedTimedAlgorithm.h"

namespace cx
{
typedef boost::shared_ptr<class Filter> FilterPtr;

/** Wrap a Filter into a TimedAlgorithm
 *
 *
 * \ingroup cxResourceAlgorithms
 * \date Nov 16, 2012
 * \author christiana
 */
class cxResourceFilter_EXPORT FilterTimedAlgorithm : public ThreadedTimedAlgorithm<bool>
{
	Q_OBJECT

public:
	FilterTimedAlgorithm(FilterPtr filter);
	virtual ~FilterTimedAlgorithm();

	FilterPtr getFilter();

protected slots:
	virtual void preProcessingSlot();
	virtual void postProcessingSlot();

private:
	virtual bool calculate();

	//  QString       mOutputBasePath;
	//  std::vector<DataPtr> mInput;
	//  std::vector<DataPtr> mOutput;
	//  QDomElement mOptions;
	FilterPtr mFilter;
};
typedef boost::shared_ptr<class FilterTimedAlgorithm> FilterTimedAlgorithmPtr;



} // namespace cx

#endif // CXFILTERTIMEDALGORITHM_H
