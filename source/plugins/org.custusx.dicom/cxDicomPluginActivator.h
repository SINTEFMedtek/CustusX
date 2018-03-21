/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXDICOMPLUGINACTIVATOR_H_
#define CXDICOMPLUGINACTIVATOR_H_

#include "org_custusx_dicom_Export.h"

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

namespace cx
{
/**
 * \defgroup org_custusx_dicom
 * \ingroup cx_plugins
 *
 * \see cx::DicomGUIExtenderService.
 *
 */

typedef boost::shared_ptr<class DicomGUIExtenderService> DicomGUIExtenderServicePtr;
typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;

/**
 * Activator for the dicom plugin
 *
 * \ingroup org_custusx_dicom
 *
 * \date 2014-04-15
 * \author Christian Askeland
 */
class org_custusx_dicom_EXPORT DicomPluginActivator :  public QObject, public ctkPluginActivator
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator)
  Q_PLUGIN_METADATA(IID "org_custusx_dicom")

public:

  DicomPluginActivator();
  ~DicomPluginActivator();

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

private:
  RegisteredServicePtr mRegistration;
};

} // namespace cx

#endif /* CXDICOMPLUGINACTIVATOR_H_ */
