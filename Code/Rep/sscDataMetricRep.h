// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#ifndef SSCDATAMETRICREP_H_
#define SSCDATAMETRICREP_H_

#include "sscRepImpl.h"
#include "sscVector3D.h"

namespace ssc
{

/**
 * \brief Base class for all Data Metric reps.
 *
 * \ingroup sscRep
 * \ingroup sscRep3D
 *
 * Handles common functionality: labels, size.
 *
 * \date Jul 31, 2011
 * \author Christian Askeland, SINTEF
 */
class DataMetricRep: public ssc::RepImpl
{
Q_OBJECT
public:
	virtual ~DataMetricRep() {}

	void setGraphicsSize(double size);
	void setLabelSize(double size);
	void setShowLabel(bool on);
	void setColor(double red, double green, double blue);

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

#endif /* SSCDATAMETRICREP_H_ */
