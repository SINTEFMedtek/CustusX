#ifndef PROBEXMLCONFIGPARSERMOC_H_
#define PROBEXMLCONFIGPARSERMOC_H_

#include <ProbeXmlConfigParser.h>

#include <QString>

/*
 * ProbeXmlConfigParserMoc.h
 *
 *  \brief Moc implementation of abstract interface ProbeXmlConfigParser
 *  All function return dummy values
 *
 *  \date Jun 4, 2013
 *  \author Ole Vegard Solberg, SINTEF
 */
class ProbeXmlConfigParserMoc : public ProbeXmlConfigParser
{
public:
	ProbeXmlConfigParserMoc(QString& pathToXml);
	virtual ~ProbeXmlConfigParserMoc();
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
};

#endif /* PROBEXMLCONFIGPARSERMOC_H_ */
