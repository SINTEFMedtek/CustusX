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

namespace cx
{

ScreenVideoProvider::ScreenVideoProvider(VisServicesPtr services) :
	mServices(services)
{

}


void ScreenVideoProvider::saveScreenShot(QPixmap pixmap, QString id)
{
	QString ending = "png";
	if (!id.isEmpty())
		ending = id + "." + ending;
	QString path = mServices->patientModelService->generateFilePath("Screenshots", ending);
	QtConcurrent::run(boost::bind(&ScreenVideoProvider::saveScreenShotThreaded, this, pixmap.toImage(), path));
}

QByteArray ScreenVideoProvider::generatePNGEncoding(QPixmap pixmap)
{
	QImage image = pixmap.toImage();

	QByteArray ba;
	QBuffer buffer(&ba);
	buffer.open(QIODevice::WriteOnly);
	image.save(&buffer, "PNG"); // writes image into ba in PNG format//	QString ending = "png";
	return ba;
}

/**Intended to be called in a separate thread.
 * \sa saveScreenShot()
 */
void ScreenVideoProvider::saveScreenShotThreaded(QImage pixmap, QString filename)
{
	pixmap.save(filename, "png");
	report("Saved screenshot to " + filename);
	reporter()->playScreenShotSound();
}

QPixmap ScreenVideoProvider::grabScreen(unsigned screenid)
{
	QDesktopWidget* desktop = qApp->desktop();
	QList<QScreen*> screens = qApp->screens();

	QWidget* screenWidget = desktop->screen(screenid);
	WId screenWinId = screenWidget->winId();
	QRect geo = desktop->screenGeometry(screenid);
	QString name = "";
	if (desktop->screenCount()>1)
	{
		name = screens[screenid]->name().split(" ").join("");
		//On windows screens[i]->name() is "\\.\DISPLAY1",
		//Have to remove unvalid chars for the filename
		name.replace("\\", "");
		name.replace(".", "");
	}
	QPixmap pixmap = screens[screenid]->grabWindow(screenWinId, geo.left(), geo.top(), geo.width(), geo.height());
	return pixmap;
}

//void ScreenVideoProvider::shootScreen()
//{
//	QDesktopWidget* desktop = qApp->desktop();
//	QList<QScreen*> screens = qApp->screens();

//	for (int i=0; i<desktop->screenCount(); ++i)
//	{
//		QPixmap pixmap = this->grabScreen(i);
////		this->saveScreenShot(screens[i]->grabWindow(screenWinId, geo.left(), geo.top(), geo.width(), geo.height()), name);
//	}

//}

} // namespace cx
