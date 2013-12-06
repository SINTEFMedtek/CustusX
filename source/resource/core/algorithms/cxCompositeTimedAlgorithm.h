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

class CompositeTimedAlgorithm : public TimedBaseAlgorithm
{
public:
	CompositeTimedAlgorithm(QString name);
	virtual void append(TimedAlgorithmPtr child);
	virtual void clear() = 0;

protected:
	std::vector<TimedAlgorithmPtr> mChildren;
};
typedef boost::shared_ptr<CompositeTimedAlgorithm> CompositeTimedAlgorithmPtr;




/**Composition of several TimedBaseAlgorithms executing in a sequence.
 *
 * Usage: Append all algorithms as children then execute. All children
 * will be executed in sequence. started()/finished() will also work.
 *
 * \ingroup cxResourceAlgorithms
 * \date Jun 27, 2012
 * \author Christian Askeland
 */
class CompositeSerialTimedAlgorithm : public CompositeTimedAlgorithm
{
	Q_OBJECT

public:
	explicit CompositeSerialTimedAlgorithm(QString name="composite");
	virtual void clear();

	virtual QString getProduct() const;
	virtual void execute();
	virtual bool isFinished() const;
    virtual bool isRunning() const;

private slots:
	void jumpToNextChild();

private:
	int mCurrent;
};
typedef boost::shared_ptr<CompositeSerialTimedAlgorithm> CompositeSerialTimedAlgorithmPtr;



/**Composition of several TimedBaseAlgorithms executing in parallel.
 *
 * Usage: Append all algorithms as children then execute. All children
 * will be executed in parallel. started()/finished() will also work.
 *
 * \ingroup cxResourceAlgorithms
 * \date Nov 06, 2012
 * \author Christian Askeland
 */
class CompositeParallelTimedAlgorithm : public CompositeTimedAlgorithm
{
	Q_OBJECT

public:
	CompositeParallelTimedAlgorithm(QString name="parallel");
	virtual void clear();

	virtual QString getProduct() const;
	virtual void execute();
	virtual bool isFinished() const;
    virtual bool isRunning() const;

private slots:
	void oneFinished();

};
typedef boost::shared_ptr<CompositeParallelTimedAlgorithm> CompositeParallelTimedAlgorithmPtr;


}

#endif /* CXCOMPOSITETIMEDALGORITHM_H_ */
