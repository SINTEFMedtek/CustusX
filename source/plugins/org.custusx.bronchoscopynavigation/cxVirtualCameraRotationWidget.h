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

#ifndef CXVIRTUALCAMERAROTATIONWIDGET_H_
#define CXVIRTUALCAMERAROTATIONWIDGET_H_

#include <QtWidgets>
#include <QDial>
#include "cxBaseWidget.h"
#include "cxVisServices.h"
#include "cxFrame3D.h"
#include "cxForwardDeclarations.h"

class QVBoxLayout;


namespace cx
{
typedef boost::shared_ptr<class StringPropertySelectTool> StringPropertySelectToolPtr;
	
/**
 * Widget for calibration virtual camera rotation
 *
 * \ingroup org_custusx_bronchoscopynavigation
 *
 * \date 2021-10-12
 * \author Erlend F. Hofstad
 */
class VirtualCameraRotationWidget : public BaseWidget
{
	Q_OBJECT
public:
	VirtualCameraRotationWidget(VisServicesPtr services, StringPropertySelectToolPtr toolSelector, QWidget* parent = 0);
	virtual ~VirtualCameraRotationWidget();
	QString getWidgetName();

private slots:
	void toolCalibrationChanged();
	void toolRotationChanged();

private:
	ToolPtr getTool();
	QString defaultWhatsThis() const;
	QVBoxLayout*  mVerticalLayout;
	StringPropertySelectToolPtr mToolSelector;
	QDial* mRotateDial;
	DecomposedTransform3D mDecomposition;
	
};
} /* namespace cx */

#endif /* CXVIRTUALCAMERAROTATIONWIDGET_H_ */
