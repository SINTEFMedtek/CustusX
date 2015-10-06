/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
