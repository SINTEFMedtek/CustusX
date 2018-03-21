/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
