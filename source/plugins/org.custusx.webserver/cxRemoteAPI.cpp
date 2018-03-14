/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxRemoteAPI.h"

#include "cxViewService.h"
#include "cxLayoutRepository.h"
#include <QStringList>
#include "cxScreenVideoProvider.h"
#include "cxLayoutVideoSource.h"
#include "cxViewCollectionWidget.h"
#include "vtkRenderWindow.h"

namespace cx
{

RemoteAPI::RemoteAPI(VisServicesPtr services) : mServices(services)
{
    mScreenVideo = new ScreenVideoProvider(mServices);
}

QStringList RemoteAPI::getAvailableLayouts() const
{
    std::vector<QString> val;
    val = mServices->view()->getLayoutRepository()->getAvailable();
    return QStringList::fromVector(QVector<QString>::fromStdVector(val));
}

void RemoteAPI::createLayoutWidget(QSize size, QString layout)
{
    mScreenVideo->showSecondaryLayout(size, layout);
}

void RemoteAPI::closeLayoutWidget()
{
    mScreenVideo->closeSecondaryLayout();
}

LayoutVideoSourcePtr RemoteAPI::startStreaming()
{
	ViewCollectionWidget* vcw = mScreenVideo->getSecondaryLayoutWidget();
	LayoutVideoSourcePtr source(new LayoutVideoSource(vcw));
    return source;
}

QImage RemoteAPI::grabLayout()
{
    QImage image = mScreenVideo->grabSecondaryLayout();
//    mScreenVideo->saveScreenShot(image, "webimage_view");
    return image;
}

QImage RemoteAPI::grabScreen()
{
    QImage image = mScreenVideo->grabScreen(0).toImage();
    //	mScreenVideo->saveScreenShot(pm, "webimage");
    return image;
}


} // namespace cx
