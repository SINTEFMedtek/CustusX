/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXLOGCONSOLE_H
#define CXLOGCONSOLE_H

#include <QMainWindow>
#include "cxXmlOptionItem.h"
#include "cxLog.h"

namespace cx
{

class LogConsole: public QMainWindow
{
	Q_OBJECT

public:
	LogConsole();
	~LogConsole();

private slots:
	void onSelectFolder();
private:
	cx::XmlOptionFile mOptions;
	cx::LogPtr mLog;

	XmlOptionItem getGeometryOption();
	void setDefaultGeometry();
	void initializeGeometry();
	void createMenus();
	void updateWindowTitle();
};

} // namespace cx


#endif // CXLOGCONSOLE_H
