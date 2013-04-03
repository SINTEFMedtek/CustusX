#include "probeXmlConfigParser.h"

#include <iostream>
#include <QFile>
#include <QStringList>
#include <algorithm>
#include <qfileinfo.h>
#include "sscMessageManager.h"
#include "sscTypeConversions.h"

//class XmlOptionFile
//{
//public:
//	static XmlOptionFile createNull(); ///< create an empty document
//	explicit XmlOptionFile(QString filename, QString name = ""); ///< create from filename, create trivial document of type name and root node if no file exists.
//	XmlOptionFile();


ProbeXmlConfigParser::ProbeXmlConfigParser(QString& pathToXml)
{
	if (!QFileInfo(pathToXml).exists())
	{
		ssc::messageManager()->sendError(QString("Cannot find probe config file [%1]").arg(pathToXml));
	}

	mFile = ssc::XmlOptionFile(pathToXml);
//  QFile file(pathToXml);
//  if(file.open(QIODevice::ReadOnly))
//  {
//    QString emsg;
//    int eline, ecolumn;
//    if (!mDomDoc.setContent(&file, false, &emsg, &eline, &ecolumn))
//    {
//      std::cout << "Could not parse XML file :";
//      std::cout << file.fileName().toStdString() << " because: ";
//      std::cout << emsg.toStdString() << std::endl;
//      throw "Could not parse XML file :";
//    }
//    file.close();
//  }
//  else
//  {
//    std::cout << "Could not open XML file :";
//    std::cout << file.fileName().toStdString() << std::endl;
//  }
  
}

ProbeXmlConfigParser::~ProbeXmlConfigParser()
{}

QStringList ProbeXmlConfigParser::getScannerList()
{
  QStringList retval;
  QList<QDomNode> scannerNodes = this->getScannerNodes();
  QDomNode node;
  foreach(node, scannerNodes)
  {
    retval << node.firstChildElement("Name").text();
  }
  return retval;
}

QStringList ProbeXmlConfigParser::getProbeList(QString scanner)
{
  QStringList retval;
  QList<QDomNode> probeNodes = this->getProbeNodes(scanner);
  QDomNode node;
  foreach(node, probeNodes)
  {
      retval << node.firstChildElement("Name").text();
  }
  return retval;
}

QStringList ProbeXmlConfigParser::getRtSourceList(QString scanner, QString probe)
{
  QStringList retval;
  QList<QDomNode> rtSourceNodes = this->getRTSourceNodes(scanner, probe);
  QDomNode node;
  foreach(node, rtSourceNodes)
  {
      retval << node.firstChildElement("Name").text();
  }
  return retval;
}

QStringList ProbeXmlConfigParser::getConfigIdList(QString scanner, QString probe, QString rtSource)
{
  QStringList retval;
  QList<QDomNode> configNodes = this->getConfigNodes(scanner, probe, rtSource);
  QDomNode node;
  foreach(node, configNodes)
  {
    retval << node.firstChildElement("ID").text();
  }
  return retval;
}

namespace
{
	QDomElement findElement(QDomNode parent, QString name)
	{
		QDomNode node = parent.namedItem(name);
		QDomElement element = node.toElement();
		if(element.isNull())
			ssc::messageManager()->sendWarning(QString("Cannot find node %2/%1").arg(name).arg(parent.toElement().tagName()));
		return element;
	}


/**Read a textnode into retval, if textnode is present
 *
 */
	template<class TYPE> bool readTextNode(TYPE* retval, QDomNode parent, QString name);

	template<>
	bool readTextNode<QString>(QString* retval, QDomNode parent, QString name)
	{
		QDomElement element = parent.namedItem(name).toElement();
		bool found = !element.isNull() && !element.text().isNull();
		if(found)
			*retval = element.text();
		if (!found)
			ssc::messageManager()->sendWarning(QString("Cannot find node %2/%1").arg(name).arg(parent.toElement().tagName()));
		return found;
	}
	template<>
	bool readTextNode<int>(int* retval, QDomNode parent, QString name)
	{
		bool ok = false;
		QDomElement element = parent.namedItem(name).toElement();
		bool found = !element.isNull() && !element.text().isNull();
		if(found)
			*retval = element.text().toInt(&ok);
		if (!found)
		{
			ssc::messageManager()->sendWarning(QString("Cannot find node %2/%1").arg(name).arg(parent.toElement().tagName()));
		}
		else
		{
			found = found && ok;
			if (!found)
				ssc::messageManager()->sendWarning(QString("Cannot convert node %2/%1 to int").arg(name).arg(parent.toElement().tagName()));
		}
		return found;
	}
	template<>
	bool readTextNode<double>(double* retval, QDomNode parent, QString name)
	{
		bool ok = false;
		QDomElement element = parent.namedItem(name).toElement();
		bool found = !element.isNull() && !element.text().isNull();
		if(found)
			*retval = element.text().toDouble(&ok);
		if (!found)
		{
			ssc::messageManager()->sendWarning(QString("Cannot find node %2/%1").arg(name).arg(parent.toElement().tagName()));
		}
		else
		{
			found = found && ok;
			if (!found)
				ssc::messageManager()->sendWarning(QString("Cannot convert node %2/%1 to double").arg(name).arg(parent.toElement().tagName()));
		}
		return found;
	}
} // unnamed namespace

ProbeXmlConfigParser::Configuration ProbeXmlConfigParser::getConfiguration(QString scanner, QString probe, QString rtsource, QString configId)
{
  Configuration retval;
//  std::cout << QString("===getConfiguration %1, %2, %3, %4").arg(scanner).arg(probe).arg(rtsource).arg(configId) << std::endl;
  
  retval.mUsScanner = scanner;
  retval.mUsProbe = probe;
  retval.mRtSource = rtsource;
  retval.mConfigId = configId;
  retval.mNotes = "Found no notes.";

  QList<QDomNode> currentScannerNodeList = this->getScannerNodes(scanner);
  if(currentScannerNodeList.isEmpty())
  {
	  ssc::messageManager()->sendWarning(QString("No scanners found [%1]").arg(scanner));
	  return retval;
  }
  QDomNode scannerNode = currentScannerNodeList.first();

  QList<QDomNode> probeList = this->getProbeNodes(scanner, probe); ///< get a list of all probes for that scanner
  if(probeList.isEmpty())
  {
	  ssc::messageManager()->sendWarning(QString("No probes found [%1/%2]").arg(scanner, probe));
	  return retval;
  }
  QDomNode probeNode = probeList.first();

//  QDomElement element;
//  readTextNode(&retval.mNotes, probeNode, "Notes");
//  std::cout << "Notes : " << retval.mNotes << std::endl;

  QList<QDomNode> currentRtSourceNodeList = this->getRTSourceNodes(scanner, probe, rtsource);
  if(currentRtSourceNodeList.isEmpty())
  {
	ssc::messageManager()->sendWarning(QString("No rtsources found [%1/%2/%3]").arg(scanner).arg(probe).arg(rtsource));
    return retval;
  }
  QDomNode rtSourceNode = currentRtSourceNodeList.first();

  // read temporal calibration. Accept platform-specific overrides as well.
  retval.mTemporalCalibration = 0;

  readTextNode(&retval.mTemporalCalibration, rtSourceNode, "TemporalCalibration");
  QString tempCalPlatformName = "None";
//  std::cout << "Base tc: " << retval.mTemporalCalibration << std::endl;
#ifdef WIN32
  tempCalPlatformName = "TemporalCalibrationWindows";
#elif __APPLE__
  tempCalPlatformName = "TemporalCalibrationApple";
#else
  tempCalPlatformName = "TemporalCalibrationLinux";
#endif
	QDomNode node = rtSourceNode.namedItem(tempCalPlatformName);
	if(!node.isNull())
		readTextNode(&retval.mTemporalCalibration, rtSourceNode, tempCalPlatformName);

	//  std::cout << "Platform tc: " << retval.mTemporalCalibration << std::endl;

    QDomElement sizeNode = findElement(rtSourceNode, "ImageSize");
    readTextNode(&retval.mImageWidth, sizeNode, "Width");
    readTextNode(&retval.mImageHeight, sizeNode, "Height");
    readTextNode(&retval.mHorizontalOffset, rtSourceNode, "HorizontalOffset");

  QList<QDomNode> currentConfigNodeList = this->getConfigNodes(scanner, probe, rtsource, configId);
  if(currentConfigNodeList.isEmpty())
  {
	ssc::messageManager()->sendWarning(QString("No nodes found in config [%1/%2/%3/%4]").arg(scanner).arg(probe).arg(rtsource).arg(configId));
    return retval;
  }
  QDomNode configNode = currentConfigNodeList.first();

    readTextNode(&retval.mName, configNode, "Name");
    readTextNode(&retval.mWidthDeg, configNode, "WidthDeg");
    if (retval.mWidthDeg > 1.0E-6)
    {
		readTextNode(&retval.mDepth, configNode, "Depth");
		readTextNode(&retval.mOffset, configNode, "Offset");
    }
    QDomElement originElement = findElement(configNode, "Origin");
    readTextNode(&retval.mOriginCol, originElement, "Col");
    readTextNode(&retval.mOriginRow, originElement, "Row");

//    element = configNode.namedItem("NCorners").toElement();
//    if(element.isNull())
//      throw "Can't find NCorners";
//    retval.mNCorners = element.text().toInt(&ok);
//    if(!ok)
//      throw "NCorners not a number";
//
//    QDomNode cornerNode = configNode.firstChildElement("Corner");
//    if(cornerNode.isNull())
//      throw "Can't find Corner";
//    retval.mCorners.clear();
//    for(int i=0; i<retval.mNCorners ; ++i)
//    {
//      element = cornerNode.namedItem("Col").toElement();
//      if(element.isNull())
//        throw "Can't find Corner.Col";
//      int col = element.text().toFloat(&ok);
//      if(!ok)
//        throw "Corner.Col not a number";
//
//      element = cornerNode.namedItem("Row").toElement();
//      if(element.isNull())
//        throw "Can't find Corner.Row";
//      int row = element.text().toFloat(&ok);
//      if(!ok)
//        throw "Corner.Row not a number";
//
//      retval.mCorners.push_back(ColRowPair(col,row));
//
//      cornerNode = cornerNode.nextSibling();
//    }
//    //sort the vector
//    ColRowPair center;
//    for(unsigned i=0;i<retval.mCorners.size();++i)
//    {
//      center.first += retval.mCorners[i].first/retval.mCorners.size();
//      center.second += retval.mCorners[i].second/retval.mCorners.size();
//    }
//    std::sort(retval.mCorners.begin(), retval.mCorners.end(), Angular_less(center));

    QDomElement edgesElement = findElement(configNode, "CroppingEdges");
    readTextNode(&retval.mLeftEdge, edgesElement, "Left");
    readTextNode(&retval.mRightEdge, edgesElement, "Right");
    readTextNode(&retval.mTopEdge, edgesElement, "Top");
    readTextNode(&retval.mBottomEdge, edgesElement, "Bottom");

    QDomElement pixelSizeElement = findElement(configNode, "PixelSize");
    readTextNode(&retval.mPixelWidth, pixelSizeElement, "Width");
    readTextNode(&retval.mPixelHeight, pixelSizeElement, "Height");

//  std::cout << "FERDIG" << std::endl;
  retval.mEmpty = false;
  return retval;
}

QList<QDomNode> ProbeXmlConfigParser::getScannerNodes(QString scanner)
{
  QList<QDomNode> retval;
  if(scanner == "ALL")
  {
    retval = this->nodeListToListOfNodes(mFile.getDocument().elementsByTagName("USScanner"));
  }
  else
  {
    QList<QDomNode> temp = this->nodeListToListOfNodes(mFile.getDocument().elementsByTagName("USScanner"));
    QDomNode node;
    foreach(node, temp)
    {
      if(node.firstChildElement("Name").text() == scanner)
       retval.append(node);
    }
  }
  return retval;
}

QList<QDomNode> ProbeXmlConfigParser::getProbeNodes(QString scanner, QString probe)
{
  QList<QDomNode> retval;
  if(probe == "ALL")
  {
    QList<QDomNode> temp = this->getScannerNodes(scanner);
    QDomNode scannerNode;
    foreach(scannerNode, temp)
    {
      retval.append(this->nodeListToListOfNodes(scannerNode.toElement().elementsByTagName("USProbe")));
    }
  }
  else
  {
    QList<QDomNode> temp = this->getScannerNodes(scanner);
    QDomNode scannerNode;
    foreach(scannerNode, temp)
    {
      QList<QDomNode> temp2 = this->nodeListToListOfNodes(scannerNode.toElement().elementsByTagName("USProbe"));
      QDomNode probeNode;
      foreach(probeNode, temp2)
      {
        if(probeNode.firstChildElement("Name").text() == probe)
          retval.append(probeNode);
      }
    }
  }
  return retval;
}

QList<QDomNode> ProbeXmlConfigParser::getRTSourceNodes(QString scanner, QString probe, QString rtSource)
{
  QList<QDomNode> retval;
  if(rtSource == "ALL")
  {
    QList<QDomNode> temp = this->getProbeNodes(scanner, probe);
    QDomNode probeNode;
    foreach(probeNode, temp)
    {
      retval.append(this->nodeListToListOfNodes(probeNode.toElement().elementsByTagName("RTSource")));
    }
  }
  else
  {
    QList<QDomNode> temp = this->getProbeNodes(scanner, probe);
    QDomNode probeNode;
    foreach(probeNode, temp)
    {
      QList<QDomNode> temp2 = this->nodeListToListOfNodes(probeNode.toElement().elementsByTagName("RTSource"));
      QDomNode rtSourceNode;
      foreach(rtSourceNode, temp2)
      {
        if(rtSourceNode.firstChildElement("Name").text() == rtSource)
          retval.append(rtSourceNode);
      }
    }
  }
  return retval;
}

QList<QDomNode> ProbeXmlConfigParser::getConfigNodes(QString scanner, QString probe, QString rtsource, QString config)
{
  QList<QDomNode> retval;
  if(config == "ALL")
  {
    QList<QDomNode> temp = this->getRTSourceNodes(scanner, probe, rtsource);
    QDomNode rtSourceNode;
    foreach(rtSourceNode, temp)
    {
      retval.append(this->nodeListToListOfNodes(rtSourceNode.toElement().elementsByTagName("Config")));
    }
  }
  else
  {
    QList<QDomNode> temp = this->getRTSourceNodes(scanner, probe, rtsource);
    QDomNode rtSourceNode;
    foreach(rtSourceNode, temp)
    {
      QList<QDomNode> temp2 = this->nodeListToListOfNodes(rtSourceNode.toElement().elementsByTagName("Config"));
      QDomNode configNode;
      foreach(configNode, temp2)
      {
        if(configNode.firstChildElement("ID").text() == config)
          retval.append(configNode);
      }
    }
  }
  return retval;
}

QList<QDomNode> ProbeXmlConfigParser::nodeListToListOfNodes(QDomNodeList list)
{
    QList<QDomNode> retval;
    for(int i=0; i < list.count(); ++i)
    {
      retval.append(list.item(i));
    }
    return retval;
}

void ProbeXmlConfigParser::removeConfig(QString scanner, QString probe, QString rtsource, QString configId)
{
	QList<QDomNode> configNodes = this->getConfigNodes(scanner, probe, rtsource, configId);
	if (configNodes.empty())
	{
		ssc::messageManager()->sendWarning(
						QString("Failed to remove probe config: No such path %1/%2/%3/%4")
						.arg(scanner)
						.arg(probe)
						.arg(rtsource)
						.arg(configId));
		return;
	}
	QDomNode victim = configNodes.first();

	QDomNode parentNode = victim.parentNode();
	parentNode.removeChild(victim);
	victim = QDomElement();// Create null element (redundant?)

	mFile.save();
}

/**Add a trivial QDomElement with a QTextElement, i.e.
 * <elem> text </elem> .
 *
 */
void ProbeXmlConfigParser::addTextElement(QDomElement parent, QString element, QString text)
{
	QDomElement node = mFile.safeGetElement(parent, element);

	while (node.hasChildNodes())
		node.removeChild(node.firstChild());

	node.appendChild(mFile.getDocument().createTextNode(text));
	mFile.save();
}

void ProbeXmlConfigParser::saveCurrentConfig(Configuration config)
{
	QList<QDomNode> rtNodes = this->getRTSourceNodes(config.mUsScanner, config.mUsProbe, config.mRtSource);
	if (rtNodes.empty())
	{
		ssc::messageManager()->sendWarning(QString("Failed to save probe config: No such path %1/%2/%3").arg(config.mUsScanner).arg(config.mUsProbe).arg(config.mRtSource));
		return;
	}
	QDomElement rtSourceNode = rtNodes.first().toElement();
	if (rtSourceNode.isNull())
		return;

	QList<QDomNode> configNodes = this->getConfigNodes(config.mUsScanner, config.mUsProbe, config.mRtSource, config.mConfigId);
	QDomElement configNode;
	if (!configNodes.empty())
	{
		configNode = configNodes.first().toElement();
	}
	else
	{
		configNode = mFile.getDocument().createElement("Config");
		rtSourceNode.appendChild(configNode);
	}

	this->addTextElement(configNode, "ID", config.mConfigId);
	this->addTextElement(configNode, "Name", config.mName);

	this->addTextElement(configNode, "WidthDeg", qstring_cast(config.mWidthDeg));
	if (config.mWidthDeg>0)
	{
		this->addTextElement(configNode, "Depth", qstring_cast(config.mDepth));
		this->addTextElement(configNode, "Offset", qstring_cast(config.mOffset));
	}

	QDomElement originNode = mFile.safeGetElement(configNode, "Origin");
	this->addTextElement(originNode, "Col", qstring_cast(config.mOriginCol));
	this->addTextElement(originNode, "Row", qstring_cast(config.mOriginRow));

	QDomElement edgesNode = mFile.safeGetElement(configNode, "CroppingEdges");
	this->addTextElement(edgesNode, "Left", qstring_cast(config.mLeftEdge));
	this->addTextElement(edgesNode, "Right", qstring_cast(config.mRightEdge));
	this->addTextElement(edgesNode, "Top", qstring_cast(config.mTopEdge));
	this->addTextElement(edgesNode, "Bottom", qstring_cast(config.mBottomEdge));

	QDomElement pixelSizeNode = mFile.safeGetElement(configNode, "PixelSize");
	this->addTextElement(pixelSizeNode, "Width", qstring_cast(config.mPixelWidth));
	this->addTextElement(pixelSizeNode, "Height", qstring_cast(config.mPixelHeight));

	mFile.save();
}




