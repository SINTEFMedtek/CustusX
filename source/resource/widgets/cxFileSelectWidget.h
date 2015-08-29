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


/*
 * sscFileSelectWidget.h
 *
 *  Created on: May 6, 2011
 *      Author: christiana
 */

#ifndef CXFILESELECTWIDGET_H_
#define CXFILESELECTWIDGET_H_

#include "cxResourceWidgetsExport.h"

#include <QWidget>
#include <QStringList>

class QComboBox;
class QToolButton;
class QAction;

namespace cx
{

/**\brief Widget for displaying and selecting a single file.
 *
 * A combo box shows all available files within the
 * current path (recursively), and a tool button is
 * used to browse for files outside this path.
 *
 * Default name filter is .mhd
 *
 * \ingroup cx_resource_widgets
 */
class cxResourceWidgets_EXPORT FileSelectWidget: public QWidget
{
Q_OBJECT

public:
	FileSelectWidget(QWidget* parent);
	QString getFilename() const;
	void setFilename(QString name);
	void setNameFilter(QStringList filter);
	void setPaths(QStringList paths);
	void setPath(QString path);
	void setFolderDepth(int depth);

	QStringList getAllFiles();

public slots:
	void refresh();

signals:
	void fileSelected(QString name);

private slots:
	void selectData();
	void currentDataComboIndexChanged(int);
	void updateComboBox();

private:
	QStringList getAllFiles(QString folder, int depth = 5);

	QComboBox* mDataComboBox;
	QToolButton* mSelectDataButton;
	QAction* mSelectDataAction;

	QString mFilename;
	QStringList mRootPaths;
	QStringList mNameFilters;
	int mFolderDepth;
};

}

#endif /* CXFILESELECTWIDGET_H_ */
