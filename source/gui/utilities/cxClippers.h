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
