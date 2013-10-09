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

#ifndef CXFILTERTIMEDALGORITHM_H
#define CXFILTERTIMEDALGORITHM_H

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
class FilterTimedAlgorithm : public ThreadedTimedAlgorithm<bool>
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
