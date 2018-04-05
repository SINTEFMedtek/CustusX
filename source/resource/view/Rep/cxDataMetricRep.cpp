/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/



#include "cxDataMetricRep.h"

#include "cxGraphicalPrimitives.h"
#include "cxView.h"
#include "cxDataMetric.h"

#include "cxVtkHelperClasses.h"

namespace cx
{

DataMetricRep::DataMetricRep() :
	RepImpl(),
	mGraphicsSize(1),
	mShowLabel(false),
	mLabelSize(2.5),
	mShowAnnotation(true)
//	mView(NULL)
{
}

void DataMetricRep::setDataMetric(DataMetricPtr value)
{
    if (mMetric)
	{
		disconnect(mMetric.get(), SIGNAL(transformChanged()), this, SLOT(setModified()));
		disconnect(mMetric.get(), SIGNAL(propertiesChanged()), this, SLOT(setModified()));
	}

    mMetric = value;

    if (mMetric)
	{
		connect(mMetric.get(), SIGNAL(propertiesChanged()), this, SLOT(setModified()));
		connect(mMetric.get(), SIGNAL(transformChanged()), this, SLOT(setModified()));
	}

    this->clear();
	this->setModified();
}

DataMetricPtr DataMetricRep::getDataMetric()
{
    return mMetric;
}

void DataMetricRep::setShowLabel(bool on)
{
	mShowLabel = on;
	this->setModified();
}

void DataMetricRep::setGraphicsSize(double size)
{
	mGraphicsSize = size;
	this->setModified();
}

void DataMetricRep::setLabelSize(double size)
{
	mLabelSize = size;
	this->setModified();
}

void DataMetricRep::setShowAnnotation(bool on)
{
	mShowAnnotation = on;
	this->setModified();
}

void DataMetricRep::clear()
{
    mText.reset();
}

void DataMetricRep::addRepActorsToViewRenderer(ViewPtr view)
{
//    mView = view;

//	vtkRendererPtr renderer = mView->getRenderer();
//	renderer->AddObserver(vtkCommand::StartEvent, this->mCallbackCommand, 1.0);

    this->clear();
	this->setModified();
}

void DataMetricRep::removeRepActorsFromViewRenderer(ViewPtr view)
{
//	vtkRendererPtr renderer = mView->getRenderer();
//	renderer->RemoveObserver(this->mCallbackCommand);

//    mView = NULL;
    this->clear();
}

void DataMetricRep::drawText()
{
	if (!this->getView())
        return;

    QString text = this->getText();

    if (text.isEmpty())
    {
        mText.reset();
        return;
    }

	if (!mText)
	{
		mText.reset(new CaptionText3D(this->getRenderer()));
	}
	mText->setColor(mMetric->getColor());
    mText->setText(text);
    mText->setPosition(mMetric->getRefCoord());
    mText->setSize(mLabelSize / 100);
}

QString DataMetricRep::getText()
{
	if (!mShowAnnotation)
		return "";
	QStringList text;
	if (mShowLabel)
		text << mMetric->getName();
	if (mMetric->showValueInGraphics())
		text << mMetric->getValueAsString();
	return text.join(" = ");
}

Vector3D DataMetricRep::getColorAsVector3D() const
{
	if (!mMetric)
		return Vector3D(1,1,1);
//	QColor color = mMetric->getColor();
//	Vector3D retval(color.redF(), color.greenF(), color.blueF());
//	return retval;
	return cx::getColorAsVector3D(mMetric->getColor());
}

}
