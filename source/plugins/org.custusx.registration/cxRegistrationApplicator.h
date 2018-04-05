/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREGISTRATIONAPPLICATOR_H
#define CXREGISTRATIONAPPLICATOR_H

#include "org_custusx_registration_Export.h"
#include <QDateTime>
#include "cxRegistrationTransform.h"
#include <map>

namespace cx
{
typedef boost::shared_ptr<class Data> DataPtr;

/**
 * Algorithms for applying registration to backend
 *
 * \ingroup org_custusx_registration
 *
 *  \date 2014-08-28
 *  \author Christian Askeland, SINTEF
 */
class org_custusx_registration_EXPORT RegistrationApplicator
{
public:

  RegistrationApplicator(const std::map<QString, DataPtr>& source);
  ~RegistrationApplicator();

  virtual void updateRegistration(QDateTime oldTime, RegistrationTransform deltaTransform);

private:
  std::map<QString, DataPtr> mSource;
  void changeParentSpace(QDateTime oldTime, std::vector<DataPtr> data, QString oldParentSpace, ParentSpace newParentSpace);
  void updateTransform(QDateTime oldTime, std::vector<DataPtr> data, RegistrationTransform delta_pre_rMd);
  void changeParentSpace(QDateTime oldTime, DataPtr data, ParentSpace newParentSpace);
  QString generateNewSpaceUid() const;
};

} // namespace cx


#endif // CXREGISTRATIONAPPLICATOR_H
