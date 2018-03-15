/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXFILEWATCHERWIDGET_H_
#define CXFILEWATCHERWIDGET_H_

#include "cxResourceWidgetsExport.h"

#include "cxBaseWidget.h"
#include "boost/shared_ptr.hpp"

class QFileSystemWatcher;
class QFile;

namespace cx
{

/**
 * \class FileWatcherWidget
 *
 * \brief Baseclass for widgets that should watch a file.
 *
 * \ingroup cx_resource_widgets
 * \date Nov 7, 2012
 * \author Janne Beate Bakeng, SINTEF
 */
class cxResourceWidgets_EXPORT FileWatcherWidget : public BaseWidget
{
	Q_OBJECT

public:
	FileWatcherWidget(QWidget* parent, QString objectName, QString windowTitle);
	virtual ~FileWatcherWidget();

public slots:
	virtual void previewFileSlot(const QString& absoluteFilePath) = 0;

protected:
	bool internalOpenNewFile(const QString absoluteFilePath);
	void watchFile(bool on);

	QFileSystemWatcher*				mFileSystemWatcher;
	boost::shared_ptr<QFile>	mCurrentFile;
};

} /* namespace cx */
#endif /* CXFILEWATCHERWIDGET_H_ */
