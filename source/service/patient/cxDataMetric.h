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


#ifndef CXDATAMETRIC_H_
#define CXDATAMETRIC_H_

#include "sscData.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxServicePatient
 * @{
 */

/**\brief Base class for all Data Metrics
 * \ingroup cxServicePatient
 *
 * Data Metrics are ssc::Data subclasses that aims to add
 * measurements to the system, such as points, angles and
 * distances.
 *
 *  \date Aug 2, 2011
 *  \author christiana
 */
class DataMetric: public ssc::Data
{
public:
	DataMetric(const QString& uid, const QString& name = "");
	virtual ~DataMetric();

	virtual QString getSpace();
};

/**
 * @}
 */
}

#endif /* CXDATAMETRIC_H_ */
