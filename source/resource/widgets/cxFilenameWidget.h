/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXFILENAMEWIDGET_H
#define CXFILENAMEWIDGET_H

#include "cxResourceWidgetsExport.h"

#include <QWidget>
#include <QSlider>
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>
#include <boost/shared_ptr.hpp>
#include "cxOptimizedUpdateWidget.h"

namespace cx
{
class FileInputWidget;
typedef boost::shared_ptr<class FilePathProperty> FilePathPropertyPtr;

/**\brief Composite widget for filename edit.
 *
 *  Accepts a StringPropertyBasePtr
 *
 * \ingroup cx_resource_widgets
 */
class cxResourceWidgets_EXPORT FilenameWidget: public OptimizedUpdateWidget
{
Q_OBJECT
public:
	FilenameWidget(QWidget* parent, FilePathPropertyPtr, QGridLayout* gridLayout = 0, int row = 0);

private slots:
	virtual void prePaintEvent();
	void editingFinished();
private:
	FileInputWidget* mFileInput;
	FilePathPropertyPtr mData;
};

} // namespace cx

#endif // CXFILENAMEWIDGET_H
