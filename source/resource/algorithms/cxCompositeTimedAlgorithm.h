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

#ifndef CXCOMPOSITETIMEDALGORITHM_H_
#define CXCOMPOSITETIMEDALGORITHM_H_

#include "cxTimedAlgorithm.h"

#include <boost/function.hpp>

namespace cx
{

/**Composition of several TimedBaseAlgorithms executing in a sequence.
 *
 * Usage: Append all algorithms as children then execute. All children
 * will be executed in sequence. started()/finished() will also work.
 *
 * \ingroup cxResourceAlgorithms
 * \date Jun 27, 2012
 * \author christiana
 */
class CompositeTimedAlgorithm : public TimedBaseAlgorithm
{
	Q_OBJECT
public:
	explicit CompositeTimedAlgorithm(QString name="composite");
	virtual QString getProduct() const;
	void append(TimedAlgorithmPtr child);
	void clear();
	virtual void execute();
	virtual bool isFinished() const;
private slots:
	void jumpToNextChild();
private:
	std::vector<TimedAlgorithmPtr> mChildren;
	int mCurrent;
};
typedef boost::shared_ptr<CompositeTimedAlgorithm> CompositeTimedAlgorithmPtr;



/**Composition of several TimedBaseAlgorithms executing in parallel.
 *
 * Usage: Append all algorithms as children then execute. All children
 * will be executed in parallel. started()/finished() will also work.
 *
 * \ingroup cxResourceAlgorithms
 * \date Nov 06, 2012
 * \author christiana
 */
class CompositeParallelTimedAlgorithm : public TimedBaseAlgorithm
{
	Q_OBJECT
public:
	CompositeParallelTimedAlgorithm(QString name="parallel");
	virtual QString getProduct() const;
	void append(TimedAlgorithmPtr child);
	void clear();
	virtual void execute();
	virtual bool isFinished() const;
private slots:
	void oneFinished();
private:
	std::vector<TimedAlgorithmPtr> mChildren;
};
typedef boost::shared_ptr<CompositeParallelTimedAlgorithm> CompositeParallelTimedAlgorithmPtr;


}

#endif /* CXCOMPOSITETIMEDALGORITHM_H_ */
