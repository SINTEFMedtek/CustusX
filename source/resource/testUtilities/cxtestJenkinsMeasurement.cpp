/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxtestJenkinsMeasurement.h"

#include <iostream>
#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "cxReporter.h"


namespace cxtest
{

JenkinsMeasurement::JenkinsMeasurement()
{}

void JenkinsMeasurement::printMeasurementWithCxReporter(QString name, QString value)
{
	cx::reporter()->initialize();
	this->createOutput(name, value);
	cx::Reporter::shutdown();
}

void JenkinsMeasurement::createOutput(QString name, QString value)
{
    QString measurement("\n<measurement><name>%1</name><value>%2</value></measurement>\n");
    measurement = measurement.arg(name).arg(value);
    cx::reporter()->sendRaw(measurement);
}

} //namespace cxtest
