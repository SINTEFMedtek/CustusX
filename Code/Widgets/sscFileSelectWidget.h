// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

/*
 * sscFileSelectWidget.h
 *
 *  Created on: May 6, 2011
 *      Author: christiana
 */

#ifndef SSCFILESELECTWIDGET_H_
#define SSCFILESELECTWIDGET_H_

#include <QWidget>
#include <QStringList>

class QComboBox;
class QToolButton;
class QAction;

namespace ssc
{

/**\brief Widget for displaying and selecting a single file.
 *
 * A combo box shows all available files within the
 * current path (recursively), and a tool button is
 * used to browse for files outside this path.
 *
 * Default name filter is .mhdmNameFilters
 *
 * \ingroup sscWidget
 */
class FileSelectWidget: public QWidget
{
Q_OBJECT

public:
	FileSelectWidget(QWidget* parent);
	QString getFilename() const;
	void setFilename(QString name);
	void setNameFilter(QStringList filter);
	void setPath(QString path);

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
	QString mRootPath;
	QStringList mNameFilters;
};

}

#endif /* SSCFILESELECTWIDGET_H_ */
