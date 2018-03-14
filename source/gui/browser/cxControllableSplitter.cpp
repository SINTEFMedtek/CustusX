/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxControllableSplitter.h"

#include <QSplitter>
#include <QVBoxLayout>
#include <QAction>
#include <QTimer>
#include "cxLogger.h"

namespace cx
{

ControllableSplitter::ControllableSplitter(XmlOptionFile options, QWidget *parent) :
	mShiftSplitterLeft(NULL),
	mShiftSplitterRight(NULL),
	mSplitterRatio(0.5),
	mOptions(options)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);

	mSplitter = new QSplitter(Qt::Horizontal);
	connect(mSplitter, &QSplitter::splitterMoved, this, &ControllableSplitter::onSplitterMoved);

	layout->addWidget(mSplitter, 1);

	mSplitterRatio = this->getSplitterRatioOption().readValue(QString::number(0.5)).toDouble();

	// must set geometry after sizes have been set, i.e. after return to the main loop:
	QTimer::singleShot(0, this, SLOT(initializeSettings()));
}

ControllableSplitter::~ControllableSplitter()
{
	this->getSplitterRatioOption().writeValue(QString::number(mSplitterRatio));
	this->getShiftStateOption().writeValue(QString::number(this->getShiftState()));
}

XmlOptionItem ControllableSplitter::getSplitterRatioOption()
{
	return XmlOptionItem("splitter_ratio", mOptions.getElement());
}

XmlOptionItem ControllableSplitter::getShiftStateOption()
{
	return XmlOptionItem("shift_state", mOptions.getElement());
}

void ControllableSplitter::addLeftWidget(QWidget *widget, QString name)
{
	mLeftName = name;
	mSplitter->insertWidget(0, widget);
}
void ControllableSplitter::addRightWidget(QWidget *widget, QString name)
{
	mRightName = name;
	mSplitter->insertWidget(1, widget);
}

void ControllableSplitter::initializeSettings()
{
	this->setShiftState(this->getShiftStateOption().readValue("0").toInt());
	this->onSplitterMoved();
}

QAction* ControllableSplitter::getMoveLeftAction()
{
	if (!mShiftSplitterLeft)
	{
		QAction* action = new QAction(QIcon(":/icons/open_icon_library/arrow-left-3.png"),
									  QString("Show %1").arg(mRightName), this);
		action->setToolTip(QString("Show more %1").arg(mRightName));
		action->setStatusTip(action->toolTip());
		connect(action, &QAction::triggered, this, &ControllableSplitter::onMoveSplitterLeft);
		mShiftSplitterLeft = action;
		this->enableActions();
	}
	return mShiftSplitterLeft;
}

QAction* ControllableSplitter::getMoveRightAction()
{
	if (!mShiftSplitterRight)
	{
		QAction* action = new QAction(QIcon(":/icons/open_icon_library/arrow-right-3.png"),
									  QString("Show %1").arg(mLeftName), this);
		action->setToolTip(QString("Show more %1").arg(mLeftName));
		action->setStatusTip(action->toolTip());
		connect(action, &QAction::triggered, this, &ControllableSplitter::onMoveSplitterRight);
		mShiftSplitterRight = action;
		this->enableActions();
	}
	return mShiftSplitterRight;
}

void ControllableSplitter::onMoveSplitterLeft()
{
	this->shiftSplitter(-1);
}

void ControllableSplitter::onMoveSplitterRight()
{
	this->shiftSplitter(+1);
}

void ControllableSplitter::onSplitterMoved()
{
	QList<int> sizes = mSplitter->sizes();
	if (this->splitterShowsBoth())
		mSplitterRatio = double(sizes[0]) /double(sizes[0]+sizes[1]);

	this->enableActions();
}

void ControllableSplitter::enableActions()
{
	if (mShiftSplitterLeft)
		mShiftSplitterLeft->setEnabled(this->getShiftState()>=0);
	if (mShiftSplitterRight)
		mShiftSplitterRight->setEnabled(this->getShiftState()<=0);
}

bool ControllableSplitter::splitterShowsBoth() const
{
	QList<int> sizes = mSplitter->sizes();
	return (( sizes.size()==2 )&&( sizes[0]!=0 )&&( sizes[1]!=0 ));
}

int ControllableSplitter::getShiftState() const
{
	QList<int> sizes = mSplitter->sizes();

	if(sizes[0]==0)
		return -1;
	else if(sizes[1]==0)
		return 1;
	else
		return 0;
}

/**
 * state = -1: show props
 * state =  0: show both
 * state =  1: show browser
 */
void ControllableSplitter::setShiftState(int shiftState)
{
	QList<int> sizes = mSplitter->sizes();

	if (shiftState<0) // show props
	{
		sizes[0] = 0;
		sizes[1] = 1;
	}
	else if (shiftState>0) // show browser
	{
		sizes[0] = 1;
		sizes[1] = 0;
	}
	else // show both
	{
		int sizesum = sizes[0]+sizes[1];
		if (sizesum==0) // if size has not been initialized
			sizesum = 1000;
		sizes[0] = mSplitterRatio * sizesum;
		sizes[1] = (1.0-mSplitterRatio) * sizesum;
	}

	mSplitter->setSizes(sizes);

	this->onSplitterMoved();
}

void ControllableSplitter::shiftSplitter(int shift)
{
	// positive shift axis goes to the right, from browser to properties

	int shiftState = this->getShiftState();
	shiftState += shift;
	this->setShiftState(shiftState);
}

}//end namespace cx

