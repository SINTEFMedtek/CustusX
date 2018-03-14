/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxResourceExport.h"
#include "cxSelectDataStringPropertyBase.h"

namespace cx {

typedef boost::shared_ptr<class PointMetric> PointMetricPtr;
typedef boost::shared_ptr<class StringPropertySelectPointMetric> StringPropertySelectPointMetricPtr;
/** Adapter that selects and stores a point metric.
 * The point metrics uid is stored internally in the adapter.
 * Use setValue/getValue plus changed() to access it.
 */
class cxResource_EXPORT StringPropertySelectPointMetric : public SelectDataStringPropertyBase
{
  Q_OBJECT
public:
        static StringPropertySelectPointMetricPtr New(PatientModelServicePtr patientModelService) { return StringPropertySelectPointMetricPtr(new StringPropertySelectPointMetric(patientModelService)); }
  virtual ~StringPropertySelectPointMetric() {}

public: // basic methods
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // interface extension
  PointMetricPtr getPointMetric();

protected:
        StringPropertySelectPointMetric(PatientModelServicePtr patientModelService);
private:
  QString mPointMetricUid;
};

} //namespace cx
