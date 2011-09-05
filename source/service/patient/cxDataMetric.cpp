/*
 * cxDataMetric.cpp
 *
 *  Created on: Aug 2, 2011
 *      Author: christiana
 */

#include <cxDataMetric.h>
#include "sscRegistrationTransform.h"

namespace cx
{

DataMetric::DataMetric(const QString& uid, const QString& name) :
	Data(uid, name)
{
	m_rMd_History = ssc::RegistrationHistory::getNullObject();
}


DataMetric::~DataMetric()
{
}

QString DataMetric::getSpace()
{
	return "";
}



}
