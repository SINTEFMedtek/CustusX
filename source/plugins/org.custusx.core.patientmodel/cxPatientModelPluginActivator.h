/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXPATIENTMODELPLUGINACTIVATOR_H_
#define CXPATIENTMODELPLUGINACTIVATOR_H_

#include "org_custusx_core_patientmodel_Export.h"

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

namespace cx
{
/**
 * \defgroup org_custusx_core_patientmodel
 * \ingroup cx_plugins
 *
 *
 * \see cx::PatientModelImplService.
 * \see cx::SessionStorageServiceImpl
 *
 */

typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;

/**
 * Activator for the patientmodel plugin
 *
 * \ingroup org_custusx_core_patientmodel
 * \date 2014-05-15
 * \author Christian Askeland
 */
class org_custusx_core_patientmodel_EXPORT PatientModelPluginActivator :  public QObject, public ctkPluginActivator
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator)
  Q_PLUGIN_METADATA(IID "org_custusx_core_patientmodel")

public:

  PatientModelPluginActivator();
  ~PatientModelPluginActivator();

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

private:
	RegisteredServicePtr mSessionStorage;
	RegisteredServicePtr mPatientModel;
};

} // namespace cx

#endif /* CXPATIENTMODELPLUGINACTIVATOR_H_ */
