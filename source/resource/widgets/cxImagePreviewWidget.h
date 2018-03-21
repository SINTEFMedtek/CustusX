/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXIMAGEPREVIEWWIDGET_H_
#define CXIMAGEPREVIEWWIDGET_H_

#include "cxFileWatcherWidget.h"

namespace cx
{

/**
 * \class ImagePreviewWidget
 *
 * \brief Widget for displaying images.
 *
 * \ingroup cx_resource_widgets
 * \date Nov 7, 2012
 * \author Janne Beate Bakeng, SINTEF
 */
class cxResourceWidgets_EXPORT ImagePreviewWidget : public FileWatcherWidget
{
	Q_OBJECT

public:
	ImagePreviewWidget(QWidget* parent);
	virtual ~ImagePreviewWidget();

	void setSize(int width, int height);

public slots:
	virtual void previewFileSlot(const QString& absoluteFilePath);

private:
	QLabel*		mDisplayLabel;
};

} /* namespace cx */
#endif /* CXIMAGEPREVIEWWIDGET_H_ */
