/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxTimedAlgorithmProgressBar.h"
#include <QtWidgets>

#include "cxTimedAlgorithm.h"

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
