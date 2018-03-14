/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXMULTIFILEINPUTWIDGET_H_
#define CXMULTIFILEINPUTWIDGET_H_

#include "cxResourceWidgetsExport.h"

#include <QWidget>
class QLabel;
class QLineEdit;
class QTextEdit;
class QToolButton;
class QGridLayout;
class QAction;

namespace cx
{

/**
 *	Widget for displaying, editing and browsing for several files.
 *  \sa FileSelectWidget FileInputWidget
 *
 * \ingroup cx_resource_widgets
 * \date 2014-04-17
 * \author christiana
 */
class cxResourceWidgets_EXPORT MultiFileInputWidget : public QWidget
{
	Q_OBJECT
public:
	MultiFileInputWidget(QWidget* parent=0);

	void setDescription(QString text);
	void setFilenames(QStringList text);
//	void addFilename(QString text);
	void setHelp(QString text);
	void setBrowseHelp(QString text);
	QStringList getFilenames() const;
	QStringList getAbsoluteFilenames() const;
	void setBasePath(QString path);
	void setUseRelativePath(bool on);

signals:
	void fileChanged();

private slots:
	void browse();
	void updateColor();
	void evaluateTextChanges();
private:
	void updateHelpInternal();
	void widgetHasBeenChanged();
	QStringList convertToAbsoluteFilenames(QStringList text) const;
	QString convertToAbsoluteFilename(QString text) const;
	QString convertFilenamesToTextEditText(QStringList files) const;
	QString convertToPresentableFilename(QString text) const;

	QStringList mLastFilenames;
	QString mBasePath;
	bool mUseRelativePath;
	QString mBaseHelp;

	QLabel* mDescription;
	QTextEdit* mFilenameEdit;
	QToolButton* mBrowseButton;
	QGridLayout* mLayout;
	QAction* mBrowseAction;
};

} /* namespace cx */
#endif /* CXMULTIFILEINPUTWIDGET_H_ */
