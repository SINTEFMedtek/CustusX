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
#include "cxViewCollectionWidget.h"
#include "vtkRenderer.h"
#include "vtkWindowToImageFilter.h"
#include "vtkRenderWindow.h"
#include "vtkPNGWriter.h"
#include "vtkUnsignedCharArray.h"
#include <QPainter>
#include "cxViewCollectionImageWriter.h"
#include <QScrollArea>

namespace cx
{

SecondaryViewLayoutWindow::SecondaryViewLayoutWindow(QWidget* parent, ViewServicePtr viewService) :
	QWidget(parent),
    mViewService(viewService),
    mSecondaryLayoutId(1)
{
	this->setLayout(new QVBoxLayout(this));
	this->layout()->setMargin(0);
	this->setWindowTitle("View Layout");
}

void SecondaryViewLayoutWindow::showEvent(QShowEvent* event)
{
    QWidget* widget = mViewService->createLayoutWidget(this, 1);
	this->layout()->addWidget(widget);
    if (mViewService->getActiveLayout(mSecondaryLayoutId).isEmpty())
		mViewService->setActiveLayout("LAYOUT_OBLIQUE_3DAnyDual_x1", 1);
}

void SecondaryViewLayoutWindow::hideEvent(QCloseEvent* event)
{
    mViewService->setActiveLayout("", mSecondaryLayoutId);
}

void SecondaryViewLayoutWindow::closeEvent(QCloseEvent *event)
{
    mViewService->setActiveLayout("", mSecondaryLayoutId);
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
void ScreenVideoProvider::showSecondaryLayout(QSize size, QString layout)
{
	if (!mTopWindow)
	{
		mTopWindow = new QWidget;
		mTopWindow->setLayout(new QVBoxLayout);
		mTopWindow->layout()->setMargin(0);

		QScrollArea* scrollArea = new QScrollArea;
		scrollArea->setBackgroundRole(QPalette::Dark);
		mTopWindow->layout()->addWidget(scrollArea);

		mSecondaryViewLayoutWindow = new SecondaryViewLayoutWindow(NULL, mServices->view());
		scrollArea->setWidget(mSecondaryViewLayoutWindow);
		scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
		scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	}
	mTopWindow->show();

	this->setWidgetToNiceSizeInLowerRightCorner(size);

    if (!layout.isEmpty())
        mServices->view()->setActiveLayout(layout, mSecondaryViewLayoutWindow->mSecondaryLayoutId);

	ViewCollectionWidget* layoutWidget = this->getSecondaryLayoutWidget();
	layoutWidget->setGridMargin(0);
	layoutWidget->setGridSpacing(0);
}

void ScreenVideoProvider::setWidgetToNiceSizeInLowerRightCorner(QSize size)
{
	QDesktopWidget* desktop = qApp->desktop();
	QList<QScreen*> screens = qApp->screens();

	QWidget* screen = desktop->screen(desktop->screenNumber(mTopWindow));
	QRect rect_s = screen->geometry();

	// default to 33% of the screen
	if (size.width()==0 || size.height()==0)
	{
		size = QSize(rect_s.width()/3, rect_s.height()/3);
	}

	// constrain widget to a max of 75% of the screen
	size = QSize(std::min<int>(size.width(), rect_s.width()*0.75),
				 std::min<int>(size.height(), rect_s.height()*0.75));
	// make sure all of scroll area is visible:
	int margin = 2;
	mTopWindow->setGeometry(QRect(QPoint(0,0),
								  QSize(size.width()+margin, size.height()+margin)));

	// reposition window to lower right corner:
	QRect rect_t = mTopWindow->frameGeometry();
	mTopWindow->move(rect_s.width()-rect_t.width(),rect_s.height()-rect_t.height());

	// set size of canvas inside widget where stuff is rendered:
	QRect rect_l = QRect(QPoint(0,0), size);
	mSecondaryViewLayoutWindow->setGeometry(rect_l);

	qDebug() << "layout t: " << mTopWindow->geometry();
	qDebug() << "layout l: " << mSecondaryViewLayoutWindow->geometry();
}

void ScreenVideoProvider::closeSecondaryLayout()
{
    mSecondaryViewLayoutWindow->hide();
    delete mSecondaryViewLayoutWindow;
}

QImage ScreenVideoProvider::grabSecondaryLayout()
{
    ViewCollectionWidget* widget = this->getSecondaryLayoutWidget();
    if (!widget)
        return QImage();

    ViewCollectionImageWriter grabber(widget);
    return ViewCollectionImageWriter::vtkImageData2QImage(grabber.grab());
}

ViewCollectionWidget* ScreenVideoProvider::getSecondaryLayoutWidget()
{
    if (!mSecondaryViewLayoutWindow)
        return NULL;
    QWidget* widget = mServices->view()->getLayoutWidget(mSecondaryViewLayoutWindow->mSecondaryLayoutId);
    ViewCollectionWidget* vcWidget = dynamic_cast<ViewCollectionWidget*>(widget);
    return vcWidget;
}

} // namespace cx
