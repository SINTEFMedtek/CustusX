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
 * cxDataMetricRep.h
 *
 *  \date Jul 31, 2011
 *      \author christiana
 */

#ifndef CXDATAMETRICREP_H_
#define CXDATAMETRICREP_H_

#include "sscRepImpl.h"
//#include "sscViewportListener.h"
#include "sscVector3D.h"

namespace cx
{

/**
 * \brief Base class for all Data Metric reps.
 * \ingroup cxServiceVisualizationRep
 *
 * Handles common functionality: labels, size.
 *
 */
class DataMetricRep: public ssc::RepImpl
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
