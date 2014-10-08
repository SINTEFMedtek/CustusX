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
