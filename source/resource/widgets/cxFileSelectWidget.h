/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
