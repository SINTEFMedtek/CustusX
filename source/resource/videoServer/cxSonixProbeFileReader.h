/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXSONIXPROBEFILEREADER_H
#define CXSONIXPROBEFILEREADER_H

#include "cxGrabberExport.h"

#include <QDomDocument>
#include <QFile>

class QString;
class QDomNode;

namespace cx {

/**
* \file
* \addtogroup cx_resource_videoserver
* @{
*/

class cxGrabber_EXPORT SonixProbeFileReader
{
public:
	SonixProbeFileReader(QString probeFile);
	bool init();
	QDomNode getProbeNode(QString probeName);///< Traverse all first childs of the probes node. Return the first node where attribute name==probeName
	QDomElement getProbes();///< Traverse top level nodes. Return first node where tag=="probes"
	bool isProbeLinear(QDomNode probeNode);
	long getProbeLenght(QDomNode probeNode);
	long getProbeParam(QDomNode probeNode, QString param);

private:
	QFile mFile;
	QDomDocument mDoc;
	QDomElement mDocElem;

	bool openFile();
	bool openDomDocument();
	bool openDocumentElement();
	QDomElement getChildWithTag(QDomNode parent, QString tagName);
};

/**
* @}
*/

}
#endif // CXSONIXPROBEFILEREADER_H
