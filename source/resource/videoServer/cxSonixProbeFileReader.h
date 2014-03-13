// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#ifndef CXSONIXPROBEFILEREADER_H
#define CXSONIXPROBEFILEREADER_H

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

class SonixProbeFileReader
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
