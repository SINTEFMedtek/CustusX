/*
 * cxDataMetricRep.h
 *
 *  Created on: Jul 31, 2011
 *      Author: christiana
 */

#ifndef CXDATAMETRICREP_H_
#define CXDATAMETRICREP_H_

#include "sscRepImpl.h"
//#include "sscViewportListener.h"
#include "sscVector3D.h"

namespace cx
{

/**Base class for all Data Metric reps.
 *
 * Handles common functionality: labels, size.
 *
 */
class DataMetricRep : public ssc::RepImpl
{
  Q_OBJECT
public:
  virtual ~DataMetricRep() {}

  void setGraphicsSize(double size);
  void setLabelSize(double size);
  void setShowLabel(bool on);

protected slots:
  virtual void changedSlot() = 0; ///< called when interals are changed: update all

protected:
  DataMetricRep(const QString& uid, const QString& name);
//  virtual void rescale() = 0; ///< called when scaling has changed: rescale text etc to keep const vp size.

  double mGraphicsSize;
  bool mShowLabel;
  double mLabelSize;
  ssc::Vector3D mColor;

//  ssc::ViewportListenerPtr mViewportListener;
};
typedef boost::shared_ptr<class DataMetricRep> DataMetricRepPtr;

}

#endif /* CXDATAMETRICREP_H_ */
