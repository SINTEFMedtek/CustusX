/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxStringPropertySelectPointMetric.h"
#include "cxPatientModelService.h"
#include "cxPointMetric.h"
#include "cxLogger.h"

namespace cx {

StringPropertySelectPointMetric::StringPropertySelectPointMetric(PatientModelServicePtr patientModelService) :
				SelectDataStringPropertyBase(patientModelService, PointMetric::getTypeName())
{
    mValueName = "Select point metric";
    mHelp = "Select a point metric";
}

bool StringPropertySelectPointMetric::setValue(const QString& value)
{
    if (value==mPointMetricUid)
        return false;
    mPointMetricUid = value;
    emit changed();
    emit dataChanged(mPointMetricUid);
    return true;
}

QString StringPropertySelectPointMetric::getValue() const
{
    return mPointMetricUid;
}

PointMetricPtr StringPropertySelectPointMetric::getPointMetric()
{
    PointMetricPtr retval = mPatientModelService->getData<PointMetric>(mPointMetricUid);
    return retval;
}

} //namespace cx
