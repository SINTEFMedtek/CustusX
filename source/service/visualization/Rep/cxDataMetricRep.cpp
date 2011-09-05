/*
 * cxDataMetricRep.cpp
 *
 *  Created on: Jul 31, 2011
 *      Author: christiana
 */

#include <cxDataMetricRep.h>

namespace cx
{

DataMetricRep::DataMetricRep(const QString& uid, const QString& name) :
    RepImpl(uid,name),
    mGraphicsSize(1),
    mShowLabel(false),
    mLabelSize(2.5),
    mColor(ssc::Vector3D(1,0,0))
{
//  mViewportListener.reset(new ssc::ViewportListener);
//  mViewportListener->setCallback(boost::bind(&DataMetricRep::rescale, this));
}

void DataMetricRep::setShowLabel(bool on)
{
  mShowLabel = on;
  this->changedSlot();
}

void DataMetricRep::setGraphicsSize(double size)
{
  mGraphicsSize = size;
  this->changedSlot();
}

void DataMetricRep::setLabelSize(double size)
{
  mLabelSize = size;
  this->changedSlot();
}


}
