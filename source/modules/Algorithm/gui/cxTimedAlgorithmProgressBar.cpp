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

#include "cxTimedAlgorithmProgressBar.h"
#include <QtGui>
#include "cxTimedAlgorithm.h"
#include "cxReporter.h"
#include "cxDisplayTimerWidget.h"

namespace cx
{

TimedAlgorithmProgressBar::TimedAlgorithmProgressBar(QWidget* parent) :
    mShowTextLabel(true)
{
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setMargin(0);

	mLabel = new QLabel;
    mLabel->hide();
    layout->addWidget(mLabel);

	mTimerWidget = new DisplayTimerWidget(this);
	mTimerWidget->setFontSize(3);
    mTimerWidget->hide();
    layout->addWidget(mTimerWidget);

	mProgressBar = new QProgressBar;
	mProgressBar->hide();
	layout->addWidget(mProgressBar);
}

void TimedAlgorithmProgressBar::attach(std::set<cx::TimedAlgorithmPtr> threads)
{
	std::set<cx::TimedAlgorithmPtr>::iterator iter;
	for(iter=threads.begin(); iter!=threads.end(); ++iter)
		this->attach(*iter);
}

void TimedAlgorithmProgressBar::detach(std::set<cx::TimedAlgorithmPtr> threads)
{
	std::set<cx::TimedAlgorithmPtr>::iterator iter;
	for(iter=threads.begin(); iter!=threads.end(); ++iter)
		this->detach(*iter);
}

void TimedAlgorithmProgressBar::attach(TimedAlgorithmPtr algorithm)
{
	if (mAlgorithm.count(algorithm))
		return;

	if (algorithm)
	{
		connect(algorithm.get(), SIGNAL(started(int)), this, SLOT(algorithmStartedSlot(int)));
		connect(algorithm.get(), SIGNAL(finished()), this, SLOT(algorithmFinishedSlot()));
		connect(algorithm.get(), SIGNAL(productChanged()), this, SLOT(productChangedSlot()));
	}

	mAlgorithm.insert(algorithm);
}

void TimedAlgorithmProgressBar::detach(TimedAlgorithmPtr algorithm)
{
	if (!mAlgorithm.count(algorithm))
		return;

	if (algorithm)
	{
		disconnect(algorithm.get(), SIGNAL(started(int)), this, SLOT(algorithmStartedSlot(int)));
		disconnect(algorithm.get(), SIGNAL(finished()), this, SLOT(algorithmFinishedSlot()));
		disconnect(algorithm.get(), SIGNAL(productChanged()), this, SLOT(productChangedSlot()));
		this->algorithmFinished(algorithm.get());
	}

	mAlgorithm.erase(algorithm);
}

void TimedAlgorithmProgressBar::productChangedSlot()
{
	TimedBaseAlgorithm* algo = dynamic_cast<TimedBaseAlgorithm*>(sender());
	QString product = "algorithm";
	if (algo)
		product = algo->getProduct();

	mLabel->setText(product);
}

void TimedAlgorithmProgressBar::setShowTextLabel(bool on)
{
    mShowTextLabel = on;
    if (mLabel->isVisible())
        mLabel->setVisible(on);
}

void TimedAlgorithmProgressBar::algorithmStartedSlot(int maxSteps)
{
	TimedBaseAlgorithm* algo = dynamic_cast<TimedBaseAlgorithm*>(sender());
	QString product = "algorithm";
	if (algo)
		product = algo->getProduct();

	mLabel->setText(product);
    mLabel->setVisible(mShowTextLabel);
	mStartedAlgos.insert(algo);

	mTimerWidget->show();
	mTimerWidget->start();

	mProgressBar->setRange(0, maxSteps);
	mProgressBar->setValue(0);
	mProgressBar->show();
}

void TimedAlgorithmProgressBar::algorithmFinishedSlot()
{
	TimedBaseAlgorithm* algo = dynamic_cast<TimedBaseAlgorithm*>(sender());
	this->algorithmFinished(algo);
}

void TimedAlgorithmProgressBar::algorithmFinished(TimedBaseAlgorithm* algo)
{
	QString product = "algorithm";
	if (algo)
		product = algo->getProduct();

	mStartedAlgos.erase(algo);
	if (!mStartedAlgos.empty())
		return;

	mProgressBar->setValue(0);
	mProgressBar->hide();
	mLabel->hide();

	mTimerWidget->hide();
	mTimerWidget->stop();
}

}
