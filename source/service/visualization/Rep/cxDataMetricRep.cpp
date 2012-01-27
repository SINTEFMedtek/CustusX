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

/*
 * cxDataMetricRep.cpp
 *
 *  \date Jul 31, 2011
 *      \author christiana
 */

#include <cxDataMetricRep.h>

namespace cx
{

DataMetricRep::DataMetricRep(const QString& uid, const QString& name) :
				RepImpl(uid, name),
				mGraphicsSize(1),
				mShowLabel(false),
				mLabelSize(2.5),
				mColor(ssc::Vector3D(1, 0, 0))
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
