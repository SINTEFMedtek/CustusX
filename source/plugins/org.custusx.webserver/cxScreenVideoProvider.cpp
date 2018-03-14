/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

	QRect rect_s = desktop->availableGeometry(mTopWindow);

	// default to 33% of the screen
	if (size.width()==0 || size.height()==0)
	{
		size = QSize(rect_s.width()/3, rect_s.height()/3);
	}

	QRect rect_full = QRect(QPoint(0,0), size);

	// constrain widget to a max of 75% of the screen
	size = QSize(std::min<int>(size.width(), rect_s.width()*0.75),
				 std::min<int>(size.height(), rect_s.height()*0.75));
	// make sure all of scroll area is visible:
	int margin = 20;
	size = QSize(size.width()+margin, size.height()+margin);
	mTopWindow->setGeometry(QRect(QPoint(0,0), size));

	// reposition window to lower right corner:
	QRect rect_t = mTopWindow->frameGeometry();
	mTopWindow->move(rect_s.topLeft()
					 + QPoint(rect_s.width(), rect_s.height())
					 - QPoint(rect_t.width(), rect_t.height()));

	// set size of canvas inside widget where stuff is rendered:
	mSecondaryViewLayoutWindow->setGeometry(rect_full);

	qDebug() << "layout onscreen: " << mTopWindow->geometry();
	qDebug() << "layout internal: " << mSecondaryViewLayoutWindow->geometry();
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
