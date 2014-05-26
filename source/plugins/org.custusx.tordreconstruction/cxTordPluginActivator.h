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

#ifndef CXTORDRECONSTRUCTIONSERVICEPLUGINACTIVATOR_H_
#define CXTORDRECONSTRUCTIONSERVICEPLUGINACTIVATOR_H_

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

namespace cx
{

typedef boost::shared_ptr<class TordReconstructionService> TordReconstructionServicePtr;

/**
 * Activator for the tord reconstruction service
 *
 * \ingroup org_custusx_tordreconstruction
 *
 * \date 2014-05-09
 * \author Janne Beate Bakeng
 */
class TordPluginActivator :  public QObject, public ctkPluginActivator
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator)

public:

  TordPluginActivator();
  ~TordPluginActivator();

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

private:
  ctkPluginContext* mContext;
  TordReconstructionServicePtr mPlugin;
};

} // namespace cx

#endif /* CXTORDRECONSTRUCTIONSERVICEPLUGINACTIVATOR_H_ */
