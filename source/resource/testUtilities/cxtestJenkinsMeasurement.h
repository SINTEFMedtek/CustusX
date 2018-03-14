/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTESTJENKINSMEASUREMENT_H
#define CXTESTJENKINSMEASUREMENT_H

#include "cxtestutilities_export.h"
#include <QString>

namespace cxtest
{

class CXTESTUTILITIES_EXPORT JenkinsMeasurement
{
public:
	JenkinsMeasurement();

	void printMeasurementWithCxReporter(QString name, QString value);///< Setup and shutdown the cx::Reporter and print the measurement. Can be used when cx::Reporter is uninitialized
	void createOutput(QString name, QString value);///< create output in a way friendly to the Jenkins measurement plugin. Can be used when cx::Reporter is initialized
};
} //namespace cxtest

#endif // CXTESTJENKINSMEASUREMENT_H
