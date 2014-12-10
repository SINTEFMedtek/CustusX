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

#include "cxCompositeTimedAlgorithm.h"

#include <QStringList>
#include "cxTypeConversions.h"
#include "cxLogger.h"

namespace cx
{

CompositeTimedAlgorithm::CompositeTimedAlgorithm(QString name) :
	TimedBaseAlgorithm(name, 20)
{
}

void CompositeTimedAlgorithm::append(TimedAlgorithmPtr child)
{
	mChildren.push_back(child);
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


CompositeSerialTimedAlgorithm::CompositeSerialTimedAlgorithm(QString name) :
	CompositeTimedAlgorithm(name),
	mCurrent(-1)
{
}

QString CompositeSerialTimedAlgorithm::getProduct() const
{
	if (mCurrent >= 0 && mCurrent < mChildren.size())
	{
		return mChildren[mCurrent]->getProduct();
	}
	return "composite";
}

void CompositeSerialTimedAlgorithm::clear()
{
	// if already started, ignore
	if (mCurrent>=0)
	{
		reportError("Attempt to restart CompositeSerialTimedAlgorithm while running failed.");
		return;
	}

	mChildren.clear();
	mCurrent = -1;
}

void CompositeSerialTimedAlgorithm::execute()
{
	// if already started, ignore
	if (mCurrent>=0)
		return;
	this->startTiming();
	mCurrent = -1;
	emit started(0);
	this->jumpToNextChild();
}

void CompositeSerialTimedAlgorithm::jumpToNextChild()
{
	// teardown old child
	if (mCurrent >= 0 && mCurrent < mChildren.size())
	{
		disconnect(mChildren[mCurrent].get(), SIGNAL(finished()), this, SLOT(jumpToNextChild()));
	}
	++mCurrent;
	// setup and run next child
	if (mCurrent >= 0 && mCurrent < mChildren.size())
	{
		connect(mChildren[mCurrent].get(), SIGNAL(finished()), this, SLOT(jumpToNextChild()));
		emit productChanged();
		mChildren[mCurrent]->execute();
	}

	// check for finished
	if (mCurrent>=mChildren.size())
	{
		mCurrent = -1;
		this->stopTiming();
		emit finished();
	}
}

bool CompositeSerialTimedAlgorithm::isFinished() const
{
    return mCurrent == -1;
}

bool CompositeSerialTimedAlgorithm::isRunning() const
{
    return mCurrent >= 0;
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

CompositeParallelTimedAlgorithm::CompositeParallelTimedAlgorithm(QString name) :
	CompositeTimedAlgorithm(name)
{
}

QString CompositeParallelTimedAlgorithm::getProduct() const
{
	QStringList products;
	for (unsigned i=0; i<mChildren.size(); ++i)
	{
		products << mChildren[i]->getProduct();
	}

	if (products.isEmpty())
		return "composite parallel";

	return products.join(", ");
}

void CompositeParallelTimedAlgorithm::clear()
{

	// if already started, ignore
	if (!this->isFinished())
	{
		reportError("Attempt to restart CompositeSerialTimedAlgorithm while running failed.");
		return;
	}

	mChildren.clear();
}

void CompositeParallelTimedAlgorithm::execute()
{
	emit aboutToStart();
	emit started(0);
	for (unsigned i=0; i<mChildren.size(); ++i)
	{
		connect(mChildren[i].get(), SIGNAL(finished()), this, SLOT(oneFinished()));
	}
	for (unsigned i=0; i<mChildren.size(); ++i)
	{
		mChildren[i]->execute();
	}
}

void CompositeParallelTimedAlgorithm::oneFinished()
{
	emit productChanged();

	if (this->isFinished())
		emit finished();
}

bool CompositeParallelTimedAlgorithm::isFinished() const
{
	int count = 0;
	for (unsigned i=0; i<mChildren.size(); ++i)
	{
		if (mChildren[i]->isFinished())
			++count;
	}
	return (count==mChildren.size());
}

bool CompositeParallelTimedAlgorithm::isRunning() const
{
    return !this->isFinished();
}



}
