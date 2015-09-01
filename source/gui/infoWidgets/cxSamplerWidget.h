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
#ifndef CXSAMPLERWIDGET_H
#define CXSAMPLERWIDGET_H

#include "cxGuiExport.h"

#include "cxBaseWidget.h"

#include "cxSpaceProperty.h"
#include "cxVector3DProperty.h"
#include "cxActiveToolProxy.h"
#include "cxPointMetric.h"

namespace cx
{

/** 
 * Utility for sampling the current tool point.
 * Displays the current tool tip position in a selected coordinate system.
 *
 * \ingroup cx_gui
 * \date feb 1, 2013, 2013
 * \author christiana
 */
class cxGui_EXPORT SamplerWidget : public BaseWidget
{
	Q_OBJECT
public:
	SamplerWidget(QWidget* parent);
	virtual ~SamplerWidget();

private slots:
	void spacesChangedSlot();
	void toggleAdvancedSlot();
protected:
	virtual void prePaintEvent();
private:
	void showAdvanced();

	QHBoxLayout* mLayout;
	SpacePropertyPtr mSpaceSelector;
	SpaceListenerPtr mListener;
	ActiveToolProxyPtr mActiveTool;
	QLineEdit* mCoordLineEdit;
	QAction* mAdvancedAction;
	QHBoxLayout* mAdvancedLayout;
	QWidget* mAdvancedWidget;
};

} // namespace cx

#endif // CXSAMPLERWIDGET_H
