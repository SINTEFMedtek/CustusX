/*
 * cxCreateProbeDataFromConfiguration.h
 *
 *  \date Nov 26, 2010
 *      \author christiana
 */

#ifndef CXCREATEPROBEDATAFROMCONFIGURATION_H_
#define CXCREATEPROBEDATAFROMCONFIGURATION_H_

#include "sscProbeData.h"
#include "probeXmlConfigParser.h"

/**
* \file
* \addtogroup cxResourceUtilities
* @{
*/

ssc::ProbeData createProbeDataFromConfiguration(ProbeXmlConfigParser::Configuration config);

/**
* @}
*/

#endif /* CXCREATEPROBEDATAFROMCONFIGURATION_H_ */
