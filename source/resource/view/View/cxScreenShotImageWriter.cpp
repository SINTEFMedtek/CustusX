/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxScreenShotImageWriter.h"


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
//#include "cxViewCollectionImageWriter.h"

namespace cx
{

namespace // unnamed
{
/**Intended to be called in a separate thread.
 * \sa saveScreenShot()
 */
void saveScreenShotThreaded(QImage pixmap, QString filename)
{
	pixmap.save(filename, "png");
	report("Saved screenshot to " + filename);
	reporter()->playScreenShotSound();
}
} // unnamed ns

ScreenShotImageWriter::ScreenShotImageWriter(PatientModelServicePtr patient) :
	mPatient(patient)
{
}

void ScreenShotImageWriter::grabAllScreensToFile()
{
	QDesktopWidget* desktop = qApp->desktop();
	QList<QScreen*> screens = qApp->screens();

	for (int i=0; i<desktop->screenCount(); ++i)
	{
		QPixmap pm = this->grab(i);
		QString name = this->getName(i);
		this->save(pm.toImage(), name);
	}
}

void ScreenShotImageWriter::save(QImage image, QString id)
{
	QString ending = "png";
	if (!id.isEmpty())
		ending = id + "." + ending;
	QString path = mPatient->generateFilePath("Screenshots", ending);
	QtConcurrent::run(boost::bind(&saveScreenShotThreaded, image, path));
}

QPixmap ScreenShotImageWriter::grab(unsigned screenid)
{
	QDesktopWidget* desktop = qApp->desktop();
	QList<QScreen*> screens = qApp->screens();

	QWidget* screenWidget = desktop->screen(screenid);
	WId screenWinId = screenWidget->winId();
	QRect geo = desktop->screenGeometry(screenid);
//	QString name = "";
//	if (desktop->screenCount()>1)
//	{
//		name = screens[screenid]->name().split(" ").join("");
//		//On windows screens[i]->name() is "\\.\DISPLAY1",
//		//Have to remove unvalid chars for the filename
//		name.replace("\\", "");
//		name.replace(".", "");
//	}
	QPixmap pixmap = screens[screenid]->grabWindow(screenWinId, geo.left(), geo.top(), geo.width(), geo.height());
	return pixmap;
}

QString ScreenShotImageWriter::getName(unsigned screenid)
{
	QDesktopWidget* desktop = qApp->desktop();
	QList<QScreen*> screens = qApp->screens();

	QString name = "";
	if (desktop->screenCount()>1)
	{
		name = screens[screenid]->name().split(" ").join("");
		//On windows screens[i]->name() is "\\.\DISPLAY1",
		//Have to remove unvalid chars for the filename
		name.replace("\\", "");
		name.replace(".", "");
	}
	return name;
}

} // namespace cx
