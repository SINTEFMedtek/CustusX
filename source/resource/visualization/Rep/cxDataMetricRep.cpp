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



#include "cxDataMetricRep.h"

#include "cxGraphicalPrimitives.h"
#include "cxView.h"
#include "cxDataMetric.h"
#include "cxLogger.h"
#include "cxVtkHelperClasses.h"

namespace cx
{

DataMetricRep::DataMetricRep(const QString& uid, const QString& name) :
	RepImpl(uid, name),
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

void DataMetricRep::addRepActorsToViewRenderer(View *view)
{
//    mView = view;

//	vtkRendererPtr renderer = mView->getRenderer();
//	renderer->AddObserver(vtkCommand::StartEvent, this->mCallbackCommand, 1.0);

    this->clear();
	this->setModified();
}

void DataMetricRep::removeRepActorsFromViewRenderer(View *view)
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
