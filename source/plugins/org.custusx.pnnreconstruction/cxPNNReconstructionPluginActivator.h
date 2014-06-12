// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXPNNRECONSTRUCTIONPLUGINACTIVATOR_H_
#define CXPNNRECONSTRUCTIONPLUGINACTIVATOR_H_

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

namespace cx
{

typedef boost::shared_ptr<class PNNReconstructionService> PNNReconstructionServicePtr;

/**
 * Activator for the PNN reconstruction plugin
 *
 * \ingroup org_custusx_pnnreconstruction
 *
 * \date 2014-06-12
 * \author Janne Beate Bakeng, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 */
class PNNReconstructionPluginActivator :  public QObject, public ctkPluginActivator
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator)

public:

  PNNReconstructionPluginActivator();
  ~PNNReconstructionPluginActivator();

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

private:
  ctkPluginContext* mContext;
  PNNReconstructionServicePtr mPlugin;
  ctkServiceRegistration mRegistration;
};

} // namespace cx

#endif /* CXPNNRECONSTRUCTIONPLUGINACTIVATOR_H_ */
