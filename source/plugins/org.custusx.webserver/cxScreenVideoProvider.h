/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXSCREENVIDEOPROVIDER_H
#define CXSCREENVIDEOPROVIDER_H

#include <QObject>
#include <QPointer>
#include <QMainWindow>
#include "vtkSmartPointer.h"
#include "cxVisServices.h"
#include "cxForwardDeclarations.h"
#include "cxScreenShotImageWriter.h"

typedef vtkSmartPointer<class vtkWindowToImageFilter> vtkWindowToImageFilterPtr;
typedef vtkSmartPointer<class vtkPNGWriter> vtkPNGWriterPtr;
typedef vtkSmartPointer<class vtkUnsignedCharArray> vtkUnsignedCharArrayPtr;

namespace cx
{

class SecondaryViewLayoutWindow: public QWidget
{
Q_OBJECT

public:
	SecondaryViewLayoutWindow(QWidget* parent, ViewServicePtr viewService);
	~SecondaryViewLayoutWindow() {}

	void tryShowOnSecondaryScreen();
    int mSecondaryLayoutId;

protected:
	virtual void showEvent(QShowEvent* event);
	virtual void hideEvent(QCloseEvent* event);
	virtual void closeEvent(QCloseEvent *event);
private:
	QString toString(QRect r) const;
	int findSmallestSecondaryScreen();

	ViewServicePtr mViewService;
};

class ScreenVideoProvider : public QObject
{
	Q_OBJECT
public:
	ScreenVideoProvider(VisServicesPtr services);

    class ViewCollectionWidget* getSecondaryLayoutWidget();
    void saveScreenShot(QImage image, QString id);
	QByteArray generatePNGEncoding(QImage image);
	QPixmap grabScreen(unsigned screenid);
    void showSecondaryLayout(QSize size, QString layout);
	QImage grabSecondaryLayout();
    void closeSecondaryLayout();
private:
	VisServicesPtr mServices;
	SecondaryViewLayoutWindow* mSecondaryViewLayoutWindow;
	QPointer<class QWidget> mTopWindow;
	ScreenShotImageWriter mWriter;
	void setWidgetToNiceSizeInLowerRightCorner(QSize size);
};

} // namespace cx


#endif // CXSCREENVIDEOPROVIDER_H
