/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXFILEINPUTWIDGET_H
#define CXFILEINPUTWIDGET_H

#include "cxResourceWidgetsExport.h"

#include <QWidget>
class QLabel;
class QLineEdit;
class QToolButton;
class QGridLayout;
class QAction;

namespace cx
{

/** 
 *	Widget for displaying, editing and browsing for a filename.
 *  \sa FileSelectWidget
 *
 * \ingroup cx_resource_widgets
 * \date 28 Oct 2013, 2013
 * \author christiana
 */
class cxResourceWidgets_EXPORT FileInputWidget : public QWidget
{
	Q_OBJECT
public:
	FileInputWidget(QWidget* parent=0);

	void setDescription(QString text);
	void setFilename(QString text);
	void setHelp(QString text);
	void setBrowseHelp(QString text);
	QString getFilename() const;
	QString getAbsoluteFilename() const;
	void setBasePath(QString path);
	void setUseRelativePath(bool on);

signals:
	void fileChanged();

private slots:
	void browse();
	void updateColor();
private:
	void updateHelpInternal();
	void widgetHasBeenChanged();

	QString mBasePath;
	bool mUseRelativePath;
	QString mBaseHelp;

	QLabel* mDescription;
	QLineEdit* mFilenameEdit;
	QToolButton* mBrowseButton;
	QGridLayout* mLayout;
	QAction* mBrowseAction;
};


} // namespace cx

#endif // CXFILEINPUTWIDGET_H
