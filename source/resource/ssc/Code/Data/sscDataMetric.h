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


#ifndef SSCDATAMETRIC_H_
#define SSCDATAMETRIC_H_

#include "sscData.h"

namespace ssc
{
/**
 * \file
 * \addtogroup sscData
 * @{
 */

/**\brief Base class for all Data Metrics
 *
 * Data Metrics are ssc::Data subclasses that aims to add
 * measurements to the system, such as points, angles and
 * distances.
 *
 * \date Aug 2, 2011
 * \author Christian Askeland, SINTEF
 */
class DataMetric: public ssc::Data
{
public:
	DataMetric(const QString& uid, const QString& name = "");
	virtual ~DataMetric();

	virtual QString getSpace();
};
typedef boost::shared_ptr<DataMetric> DataMetricPtr;

/**
 * @}
 */
}

#endif /* SSCDATAMETRIC_H_ */
