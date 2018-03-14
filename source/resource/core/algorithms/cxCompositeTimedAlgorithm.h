/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXCOMPOSITETIMEDALGORITHM_H_
#define CXCOMPOSITETIMEDALGORITHM_H_

#include "cxResourceExport.h"

#include "cxTimedAlgorithm.h"

#include <boost/function.hpp>

namespace cx
{

/**
 *  \ingroup cx_resource_core_algorithms
 */
class cxResource_EXPORT CompositeTimedAlgorithm : public TimedBaseAlgorithm
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
 * \ingroup cx_resource_core_algorithms
 * \date Jun 27, 2012
 * \author Christian Askeland
 */
class cxResource_EXPORT CompositeSerialTimedAlgorithm : public CompositeTimedAlgorithm
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
 * \ingroup cx_resource_core_algorithms
 * \date Nov 06, 2012
 * \author Christian Askeland
 */
class cxResource_EXPORT CompositeParallelTimedAlgorithm : public CompositeTimedAlgorithm
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
