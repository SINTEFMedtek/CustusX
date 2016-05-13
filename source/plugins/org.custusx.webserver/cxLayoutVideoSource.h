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
#ifndef CXLAYOUTVIDEOSOURCE_H
#define CXLAYOUTVIDEOSOURCE_H

#include "cxVideoSource.h"
#include "org_custusx_webserver_Export.h"
#include <QPointer>

namespace cx
{
class ViewCollectionWidget;

/**
 * Stream images rendered to the input ViewCollectionWidget.
 */
class org_custusx_webserver_EXPORT LayoutVideoSource : public VideoSource
{
    Q_OBJECT
public:
    explicit LayoutVideoSource(ViewCollectionWidget* widget);


    virtual QString getUid();
    virtual QString getName();
    virtual vtkImageDataPtr getVtkImageData();
    virtual double getTimestamp();
	virtual TimeInfo getAdvancedTimeInfo();

    virtual QString getInfoString() const { return ""; }
    virtual QString getStatusString() const { return ""; }

    virtual void start();
    virtual void stop();

    virtual bool validData() const;
    virtual bool isConnected() const;
    virtual bool isStreaming() const;

private:
    QPointer<ViewCollectionWidget> mWidget;
    void onRendered();
    vtkImageDataPtr mGrabbed;
    QDateTime mTimestamp;
    bool mStreaming;
};

} // namespace cx

#endif // CXLAYOUTVIDEOSOURCE_H
