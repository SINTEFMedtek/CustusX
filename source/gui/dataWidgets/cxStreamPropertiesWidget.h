/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXSTREAMPROPERTIESWIDGET_H
#define CXSTREAMPROPERTIESWIDGET_H

#include "cxGuiExport.h"
#include "cxTabbedWidget.h"

namespace cx
{
typedef boost::shared_ptr<class StringPropertySelectTrackedStream> StringPropertySelectTrackedStreamPtr;
typedef boost::shared_ptr<class TransferFunction3DWidget> TransferFunction3DWidgetPtr;
typedef boost::shared_ptr<class ShadingWidget> ShadingWidgetPtr;
typedef boost::shared_ptr<class TrackedStream> TrackedStreamPtr;

/**
 * \brief Widget for displaying and manipulating TrackedStream properties.
 * \ingroup cx_gui
 * \date Feb 18, 2015
 * \author Ole Vegard Solberg, SINTEF
 */
class cxGui_EXPORT StreamPropertiesWidget : public TabbedWidget
{
	Q_OBJECT
public:
	StreamPropertiesWidget(PatientModelServicePtr patientModelService, ViewServicePtr viewService, QWidget* parent);

private slots:
	void streamSelectedSlot();
	void streamingSlot(bool isStreaming);
	void firstFrame();
private:
	StringPropertySelectTrackedStreamPtr mSelectStream;
	TransferFunction3DWidgetPtr mTransferFunctionWidget;
	ShadingWidgetPtr mShadingWidget;
	TrackedStreamPtr mTrackedStream;
};
} //cx

#endif // CXSTREAMPROPERTIESWIDGET_H
