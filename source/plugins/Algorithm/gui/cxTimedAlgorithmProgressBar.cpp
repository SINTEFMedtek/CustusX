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

#include "cxTimedAlgorithmProgressBar.h"
#include <QtGui>
#include "cxTimedAlgorithm.h"


namespace cx
{

TimedAlgorithmProgressBar::TimedAlgorithmProgressBar(QWidget* parent) :
	mStartedAlgos(0)
{
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setMargin(0);

	mLabel = new QLabel;
	layout->addWidget(mLabel);

	mProgressBar = new QProgressBar;
	mProgressBar->hide();
	layout->addWidget(mProgressBar);
}

void TimedAlgorithmProgressBar::attach(TimedAlgorithmPtr algorithm)
{
	if (mAlgorithm.count(algorithm))
		return;

	if (algorithm)
	{
		connect(algorithm.get(), SIGNAL(started(int)), this, SLOT(algorithmStartedSlot(int)));
		connect(algorithm.get(), SIGNAL(finished()), this, SLOT(algorithmFinishedSlot()));
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
	}

	mAlgorithm.erase(algorithm);
}

void TimedAlgorithmProgressBar::algorithmStartedSlot(int maxSteps)
{
	TimedAlgorithm* algo = dynamic_cast<TimedAlgorithm*>(sender());
	QString product = "algorithm";
	if (algo)
		product = algo->getProduct();

	mLabel->setText(product);
	mLabel->show();
	mStartedAlgos++;

	mProgressBar->setRange(0, maxSteps);
	mProgressBar->setValue(0);
	mProgressBar->show();
	ssc::messageManager()->sendInfo(QString("Executing %1, please wait!").arg(product));
}

void TimedAlgorithmProgressBar::algorithmFinishedSlot()
{
	TimedAlgorithm* algo = dynamic_cast<TimedAlgorithm*>(sender());
	QString product = "algorithm";
	if (algo)
		product = algo->getProduct();

	ssc::messageManager()->sendSuccess(QString("%1 done!").arg(product));

	if (--mStartedAlgos >0) // dont hide before the last algo has completed.
		return;

	mProgressBar->setValue(0);
	mProgressBar->hide();
	mLabel->hide();
}

}
