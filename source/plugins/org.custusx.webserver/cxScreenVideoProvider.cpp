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
#include "cxScreenVideoProvider.h"

#include <QPixmap>
#include "cxPatientModelService.h"
#include <QtConcurrent>
#include <QDesktopWidget>
#include <QApplication>
#include "cxReporter.h"
#include "boost/bind.hpp"
#include <QScreen>
#include <QVBoxLayout>
#include "cxViewService.h"
//#include "cxSecondaryViewLayoutWindow.h"
#include "cxViewCollectionWidget.h"
#include "vtkRenderer.h"
#include "vtkWindowToImageFilter.h"
#include "vtkRenderWindow.h"
#include "vtkPNGWriter.h"
#include "vtkUnsignedCharArray.h"
#include <QPainter>
#include "cxViewCollectionImageWriter.h"

namespace cx
{

SecondaryViewLayoutWindow::SecondaryViewLayoutWindow(QWidget* parent, ViewServicePtr viewService) :
	QWidget(parent),
	mViewService(viewService)
{
	this->setLayout(new QVBoxLayout(this));
	this->layout()->setMargin(0);
	this->setWindowTitle("View Layout");
}

void SecondaryViewLayoutWindow::showEvent(QShowEvent* event)
{
    QWidget* widget = mViewService->createLayoutWidget(this, 1);
	this->layout()->addWidget(widget);
	if (mViewService->getActiveLayout(1).isEmpty())
		mViewService->setActiveLayout("LAYOUT_OBLIQUE_3DAnyDual_x1", 1);
}

void SecondaryViewLayoutWindow::hideEvent(QCloseEvent* event)
{
	mViewService->setActiveLayout("", 1);
}

void SecondaryViewLayoutWindow::closeEvent(QCloseEvent *event)
{
	mViewService->setActiveLayout("", 1);
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

ScreenVideoProvider::ScreenVideoProvider(VisServicesPtr services) :
	mServices(services),
	mWriter(services->patient())
{

}


void ScreenVideoProvider::saveScreenShot(QImage image, QString id)
{
	mWriter.save(image,id);
}

QByteArray ScreenVideoProvider::generatePNGEncoding(QImage image)
{
	QByteArray ba;
	QBuffer buffer(&ba);
	buffer.open(QIODevice::WriteOnly);
	image.save(&buffer, "PNG"); // writes image into ba in PNG format//	QString ending = "png";
	return ba;
}

QPixmap ScreenVideoProvider::grabScreen(unsigned screenid)
{
	return mWriter.grab(screenid);
}


\
void ScreenVideoProvider::showSecondaryLayout()
{
	std::cout << "show window" << std::endl;
	if (!mSecondaryViewLayoutWindow)
		mSecondaryViewLayoutWindow = new SecondaryViewLayoutWindow(NULL, mServices->view());
	mSecondaryViewLayoutWindow->show();

	QRect rect = QRect(QPoint(50,50), QSize(320,568));
	mSecondaryViewLayoutWindow->setGeometry(rect);
	mSecondaryViewLayoutWindow->move(rect.topLeft());
}

QImage ScreenVideoProvider::grabSecondaryLayout()
{
    QWidget* widget = mServices->view()->getLayoutWidget(1);
    if (!widget)
        return QImage();
	ViewCollectionWidget* vcWidget = dynamic_cast<ViewCollectionWidget*>(widget);

	ViewCollectionImageWriter grabber(vcWidget);
    return ViewCollectionImageWriter::vtkImageData2QImage(grabber.grab());
}


} // namespace cx
