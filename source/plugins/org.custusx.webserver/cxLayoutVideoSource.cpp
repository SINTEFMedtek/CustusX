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
#include "cxLayoutVideoSource.h"
#include "cxViewCollectionImageWriter.h"
#include "cxViewCollectionWidget.h"
#include "vtkImageData.h"

namespace cx
{

LayoutVideoSource::LayoutVideoSource(ViewCollectionWidget* widget) :
    mWidget(widget),
    mStreaming(false)
{
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
