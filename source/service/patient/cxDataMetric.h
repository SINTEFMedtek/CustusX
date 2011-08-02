/*
 * cxDataMetric.h
 *
 *  Created on: Aug 2, 2011
 *      Author: christiana
 */

#ifndef CXDATAMETRIC_H_
#define CXDATAMETRIC_H_

#include "sscData.h"

namespace cx
{

/**Base class for all Data Metrics
 *
 */
class DataMetric : public ssc::Data
{
public:
	DataMetric(const QString& uid, const QString& name="");
	virtual ~DataMetric();
};

}

#endif /* CXDATAMETRIC_H_ */
