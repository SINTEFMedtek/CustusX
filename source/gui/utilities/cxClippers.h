/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXCLIPPERS_H
#define CXCLIPPERS_H

#include "cxGuiExport.h"

#include <QStringList>
#include "cxForwardDeclarations.h"
class QDomNode;

namespace cx {

/**\brief Clipper container. Used by ClippersWidget.
 *
 *  \date Oct, 2015
 *  \author Ole Vegard Solberg, SINTEF
 */
class cxGui_EXPORT Clippers : public QObject
{
	Q_OBJECT
public:
	Clippers(VisServicesPtr services);
	void importList(QString clippers);
	QString exportList();
	InteractiveClipperPtr getClipper(QString clipperName);
	void add(QString clipperName, InteractiveClipperPtr clipper);
	void remove(QString clipperName);
	bool exists(QString clipperName);
	int size() {return mClippers.size();}
	QStringList getClipperNames();

	void parseXml(QDomNode parentNode);
	void addXml(QDomNode &parentNode);
signals:
	void changed();

protected:
	VisServicesPtr mServices;
//	PatientStorage mStorage;
	std::map<QString, InteractiveClipperPtr> mClippers;
	QStringList mClipperList;//remove
//	StringPropertyPtr mClipperList;

	void createDefaultClippers();
	QStringList getInitialClipperNames();
};

}//cx

#endif // CXCLIPPERS_H
