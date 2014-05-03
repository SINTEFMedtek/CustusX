/*
 * BackendImplActivator.h
 *
 *  Created on: Apr 15, 2014
 *      Author: christiana
 */

#ifndef CXDICOMPLUGINACTIVATOR_H_
#define CXDICOMPLUGINACTIVATOR_H_

#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"

namespace cx
{
//class BackendInterface;
typedef boost::shared_ptr<class DicomPlugin> DicomPluginPtr;

class DicomPluginActivator :  public QObject, public ctkPluginActivator
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator)

public:

  DicomPluginActivator();
  ~DicomPluginActivator();

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

//  static DicomPluginActivator* getInstance();
//  ctkPluginContext* getPluginContext() const;

private:

//  static BackendImplActivator* instance;
  ctkPluginContext* context;
  DicomPluginPtr mDicomPlugin;

//  TestService* mTestService;
//  BackendInterface* mBackendInterface;

}; // TestPluginActivator

} // namespace cx

#endif /* CXDICOMPLUGINACTIVATOR_H_ */
