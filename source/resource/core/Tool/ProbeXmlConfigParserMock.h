/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef PROBEXMLCONFIGPARSERMOC_H_
#define PROBEXMLCONFIGPARSERMOC_H_

#include "cxResourceExport.h"

#include <ProbeXmlConfigParser.h>

#include <QString>

/**
 * ProbeXmlConfigParserMoc.h
 *
 *  \brief Moc implementation of abstract interface ProbeXmlConfigParser
 *  All function return dummy values
 *
 * \ingroup cx_resource_core_tool
 *  \date Jun 4, 2013
 *  \author Ole Vegard Solberg, SINTEF
 */
class cxResource_EXPORT ProbeXmlConfigParserMock : public ProbeXmlConfigParser
{
public:
	ProbeXmlConfigParserMock(QString& pathToXml, bool provideRTSource = true);
	virtual ~ProbeXmlConfigParserMock();
	virtual QString getFileName();
	virtual void removeConfig(QString scanner, QString probe, QString rtsource, QString configId);
	virtual void saveCurrentConfig(ProbeXmlConfigParser::Configuration config);
	virtual QStringList getScannerList();
	virtual QStringList getProbeList(QString scanner);
	virtual QStringList getRtSourceList(QString scanner, QString probe);
	virtual QStringList getConfigIdList(QString scanner, QString probe, QString rtSource);
	virtual ProbeXmlConfigParser::Configuration getConfiguration(QString scanner, QString probe, QString rtsource, QString configId);
private:
	QString mFileName;
	bool mProvideRTSource;
};

#endif /* PROBEXMLCONFIGPARSERMOC_H_ */
