/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxLayoutVideoSource.h"
#include "cxViewCollectionImageWriter.h"
#include "cxViewCollectionWidget.h"
#include "vtkImageData.h"
#include "cxLogger.h"

namespace cx
{

LayoutVideoSource::LayoutVideoSource(ViewCollectionWidget* widget) :
    mWidget(widget),
    mStreaming(false)
{
	CX_ASSERT(widget);
    connect(mWidget.data(), &ViewCollectionWidget::rendered, this, &LayoutVideoSource::onRendered);
}

QString LayoutVideoSource::getUid()
{
    return QString("LayoutVideoSource%1").arg(reinterpret_cast<long>(this));
}

QString LayoutVideoSource::getName()
{
    return QString("LayoutVideoSource");
}

double LayoutVideoSource::getTimestamp()
{
	return mTimestamp.toMSecsSinceEpoch();
}

TimeInfo LayoutVideoSource::getAdvancedTimeInfo()
{
	return TimeInfo(mTimestamp.toMSecsSinceEpoch());
}

void LayoutVideoSource::start()
{
    if (mStreaming)
        return;

    mStreaming = true;
    emit streaming(mStreaming);
}

void LayoutVideoSource::stop()
{
    if (!mStreaming)
        return;

    mGrabbed = vtkImageDataPtr();
    mStreaming = false;
    emit streaming(mStreaming);
}

bool LayoutVideoSource::validData() const
{
    return mStreaming;
}

bool LayoutVideoSource::isConnected() const
{
    return true;
}

bool LayoutVideoSource::isStreaming() const
{
    return mStreaming;
}

void LayoutVideoSource::onRendered()
{
    if (!mStreaming)
        return;

    mGrabbed = vtkImageDataPtr();
    mTimestamp = QDateTime::currentDateTime();
    emit newFrame();
}

vtkImageDataPtr LayoutVideoSource::getVtkImageData()
{
    if (!mStreaming)
        return vtkImageDataPtr();

    if (!mGrabbed)
    {
        ViewCollectionImageWriter grabber(mWidget);
        mGrabbed = grabber.grab();
    }
    return mGrabbed;
}

} // namespace cx
