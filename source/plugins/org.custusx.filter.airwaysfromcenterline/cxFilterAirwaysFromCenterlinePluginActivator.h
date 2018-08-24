/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXFILTERAIRWAYSFROMCENTERLINEPLUGINACTIVATOR_H_
#define CXFILTERAIRWAYSFROMCENTERLINEPLUGINACTIVATOR_H_

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

namespace cx
{

typedef boost::shared_ptr<class RegisteredService> RegisteredServicePtr;

/**
 * Activator for the AirwaysFromCenterline plugin
 *
 * \ingroup org_custusx_filter_airwaysfromcenterline
 *
 * \date 2018-05-28
 * \author Erlend Fagertun Hofstad, SINTEF
 */
class FilterAirwaysFromCenterlinePluginActivator :  public QObject, public ctkPluginActivator
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator)
  Q_PLUGIN_METADATA(IID "org_custusx_filter_airwaysfromcenterline")

public:

  FilterAirwaysFromCenterlinePluginActivator();
  ~FilterAirwaysFromCenterlinePluginActivator();

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

private:
	RegisteredServicePtr mRegistration;
};

} // namespace cx

#endif /* CXFILTERAIRWAYSFROMCENTERLINEPLUGINACTIVATOR_H_ */
