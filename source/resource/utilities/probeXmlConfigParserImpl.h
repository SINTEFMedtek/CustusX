#ifndef PROBEXMLCONFIGPARSERIMPL_H_
#define PROBEXMLCONFIGPARSERIMPL_H_

#include <probeXmlConfigParser.h>

/*
 * probeXmlConfigParserImpl.h
 *
 *  \brief Implementation of abstract interface ProbeXmlConfigParser
 *  Interface to ProbeCalibConfigs.xml
 *
 *  \date Jun 4, 2013
 *  \author Ole Vegard Solberg, SINTEF
 *  \author jbake
 */
class ProbeXmlConfigParserImpl : public ProbeXmlConfigParser
{
public:
	ProbeXmlConfigParserImpl(QString& pathToXml); ///< opens the file and reads it onto the QDomDocument
	virtual ~ProbeXmlConfigParserImpl();
	virtual QString getFileName();
	virtual void removeConfig(QString scanner, QString probe, QString rtsource, QString configId);
	virtual void saveCurrentConfig(Configuration config);
	virtual QStringList getScannerList(); ///< get a list of all scanner in the xml
	virtual QStringList getProbeList(QString scanner); ///< get a list of all probes for that scanner
	virtual QStringList getRtSourceList(QString scanner, QString probe); ///< get a list of rt sources for that scanner/probe combo
	virtual QStringList getConfigIdList(QString scanner, QString probe, QString rtSource); ///< get a list of config ids for that scanner/probe/rsource combo
	virtual Configuration getConfiguration(QString scanner, QString probe, QString rtsource, QString configId); ///< get a easy-to-work-with struct of a specific config
protected:
	QList<QDomNode> getScannerNodes(QString scanner = "ALL"); ///< get a list of ALL scanner nodes or just the one you are looking for
	QList<QDomNode> getProbeNodes(QString scanner, QString probe = "ALL"); ///< get a list of ALL probenodes for that scanner, or just the one you are looking for
	QList<QDomNode> getRTSourceNodes(QString scanner, QString probe, QString rtSource="ALL"); ///< get a list of ALL rtsourcenodes for that scanner/probe combo, or just the one you are looking for
	QList<QDomNode> getConfigNodes(QString scanner, QString probe, QString rtsource, QString config="ALL"); ///< get a list of ALL confignodes for that scanner/probe/rtsource combo, or just the one you are looking for
	QList<QDomNode> nodeListToListOfNodes(QDomNodeList list); ///< converts a QDomNodeList to a QList<QDomNode>
	void addTextElement(QDomElement parent, QString element, QString text);

	ssc::XmlOptionFile mFile;
};

//------------------------------------------------------------------------------
/*
http://stackoverflow.com/questions/1709283/how-can-i-sort-a-coordinate-list-for-a-rectangle-counterclockwise

I need to sort a coordinate list for a rectangle counterclockwise, and make the north-east corner the first coordinate.

solution seems pretty straightforward:

>>> import math
>>> mlat = sum(x['lat'] for x in l) / len(l)
>>> mlng = sum(x['lng'] for x in l) / len(l)
>>> def algo(x):
    return (math.atan2(x['lat'] - mlat, x['lng'] - mlng) + 2 * math.pi) % 2*math.pi

>>> l.sort(key=algo)
basically, algo normalises the input into the [0, 2pi] space and it would be naturally sorted "counter-clockwise".
*/

//------------------------------------------------------------------------------
class Angular_less
{
public:
  Angular_less(ProbeXmlConfigParser::ColRowPair center) : mCenter(center)
  {
  }

  bool operator()(const ProbeXmlConfigParser::ColRowPair& rhs, const ProbeXmlConfigParser::ColRowPair& lhs)
  {
    return angle(rhs) < angle(lhs);
  }

  int angle(ProbeXmlConfigParser::ColRowPair pair1)
  {
	double pi = atan(1.0)*4;
    return fmod(  ( ( atan2((double)(pair1.first - mCenter.first), (double)(pair1.second - mCenter.second)) ) + 3*pi/4) , 2*pi);
  }
  ProbeXmlConfigParser::ColRowPair mCenter;
};

/**
* @}
*/
#endif /* PROBEXMLCONFIGPARSERIMPL_H_ */
