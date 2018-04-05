/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXSCREENSHOTIMAGEWRITER_H
#define CXSCREENSHOTIMAGEWRITER_H

#include <QObject>
#include <QPointer>
#include <QMainWindow>
#include "vtkSmartPointer.h"
#include "cxVisServices.h"
#include "cxForwardDeclarations.h"

namespace cx
{
typedef boost::shared_ptr<class ScreenShotImageWriter> ScreenShotImageWriterPtr;

/**
 *
 */
class cxResourceVisualization_EXPORT ScreenShotImageWriter
{
public:
	static ScreenShotImageWriterPtr create(PatientModelServicePtr patient) { return ScreenShotImageWriterPtr(new ScreenShotImageWriter(patient)); }
	void grabAllScreensToFile();

	void save(QImage image, QString id);
	QString getName(unsigned screenid);
	QPixmap grab(unsigned screenid);

	ScreenShotImageWriter(PatientModelServicePtr patient);

private:
	PatientModelServicePtr mPatient;
};

} // namespace cx

#endif // CXSCREENSHOTIMAGEWRITER_H
