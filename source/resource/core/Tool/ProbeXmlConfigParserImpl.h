/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef PROBEXMLCONFIGPARSERIMPL_H_
#define PROBEXMLCONFIGPARSERIMPL_H_

#include "cxResourceExport.h"

#include <ProbeXmlConfigParser.h>

/*
 * probeXmlConfigParserImpl.h
 *
 *  \brief Implementation of abstract interface ProbeXmlConfigParser
 *  Interface to ProbeCalibConfigs.xml
 *
 *  \ingroup cx_resource_core_tool
 *  \date Jun 4, 2013
 *  \author Ole Vegard Solberg, SINTEF
 *  \author jbake
 */
class cxResource_EXPORT ProbeXmlConfigParserImpl : public ProbeXmlConfigParser
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

	cx::XmlOptionFile mFile;
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
class cxResource_EXPORT Angular_less
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
