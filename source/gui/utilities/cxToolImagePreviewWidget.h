/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTOOLIMAGEPREVIEWWIDGET_H_
#define CXTOOLIMAGEPREVIEWWIDGET_H_

#include "cxGuiExport.h"

#include "cxImagePreviewWidget.h"
namespace cx
{
/**
 * \class ToolImagePreviewWidget
 *
 * \brief Widget for displaying a tools image.
 *
 * \date Nov 7, 2012
 * \author Janne Beate Bakeng, SINTEF
 */
class cxGui_EXPORT ToolImagePreviewWidget : public ImagePreviewWidget
{
	Q_OBJECT

public:
	ToolImagePreviewWidget(TrackingServicePtr trackingService, QWidget* parent);
	virtual ~ToolImagePreviewWidget();

public slots:
	virtual void previewFileSlot(const QString& absoluteFilePath);

private:
	TrackingServicePtr mTrackingService;
};
} //namespace cx
#endif /* CXTOOLIMAGEPREVIEWWIDGET_H_ */
