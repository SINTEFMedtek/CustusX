/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXFILTERACCUSURFPLUGINACTIVATOR_H_
#define CXFILTERACCUSURFPLUGINACTIVATOR_H_

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

namespace cx
{

typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;

/**
 * Activator for the Accusurf reconstruction plugin
 *
 * \ingroup org_custusx_pnnreconstruction
 *
 * \date 2016-10-16
 * \author Erlend Fagertun Hofstad, SINTEF
 */
class FilterAccusurfPluginActivator :  public QObject, public ctkPluginActivator
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator)
  Q_PLUGIN_METADATA(IID "org_custusx_filter_accusurf")

public:

  FilterAccusurfPluginActivator();
  ~FilterAccusurfPluginActivator();

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

private:
	RegisteredServicePtr mRegistration;
};

} // namespace cx

#endif /* CXFILTERACCUSURFPLUGINACTIVATOR_H_ */
