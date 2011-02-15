/*
 * cxProbeAdapterRTSource.h
 *
 *  Created on: Feb 11, 2011
 *      Author: christiana
 */

#ifndef CXPROBEADAPTERRTSOURCE_H_
#define CXPROBEADAPTERRTSOURCE_H_

class QString;
#include "sscForwardDeclarations.h"
#include "vtkForwardDeclarations.h"
#include "sscRTSource.h"
#include "sscTool.h"

namespace ssc
{

/**RTSource that applies the parameters from a probe to the rtsource.
 *
 */
class ProbeAdapterRTSource : public ssc::RTSource
{
  Q_OBJECT
public:
  ProbeAdapterRTSource(QString uid, ProbePtr probe, RTSourcePtr source);
  virtual ~ProbeAdapterRTSource() {}

  virtual QString getUid();
  virtual QString getName() { return mBase->getName(); }
  virtual vtkImageDataPtr getVtkImageData();
  virtual double getTimestamp();

  virtual QString getInfoString() const { return mBase->getInfoString(); }
  virtual QString getStatusString() const { return mBase->getStatusString(); }

  virtual void start() { mBase->start(); }
  virtual void stop() { mBase->stop(); }

  virtual bool validData() const { return mBase->validData(); };
  virtual bool isConnected() const { return mBase->isConnected(); }
  virtual bool isStreaming() const { return mBase->isStreaming(); }

private slots:
  void probeChangedSlot();

private:
  QString mUid;
  ssc::RTSourcePtr mBase;
  ssc::ProbePtr mProbe;
  vtkImageChangeInformationPtr mRedirecter;
};

}

#endif /* CXPROBEADAPTERRTSOURCE_H_ */
