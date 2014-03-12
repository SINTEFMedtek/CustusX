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
#ifndef CXFILEINPUTWIDGET_H
#define CXFILEINPUTWIDGET_H

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
class FileInputWidget : public QWidget
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
