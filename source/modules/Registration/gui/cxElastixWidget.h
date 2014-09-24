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

#ifndef CXELASTIXWIDGET_H_
#define CXELASTIXWIDGET_H_

#include "cxPluginRegistrationExport.h"

#include "cxRegistrationBaseWidget.h"
#include "cxStringDataAdapter.h"
#include "cxFileSelectWidget.h"
#include "cxElastixManager.h"
#include "cxFilePreviewWidget.h"
#include "cxBoolDataAdapterXml.h"

class QSpinBox;
class QPushButton;
class QLabel;
class QLineEdit;

namespace cx
{
class TimedAlgorithmProgressBar;

/**
 * \file
 * \addtogroup cx_module_registration
 * @{
 */

/**
 * \brief Widget for interfacing to the ElastiX registration package.
 *
 * \date Feb 4, 2012
 * \author Christian Askeland, SINTEF
 */
class cxPluginRegistration_EXPORT ElastixWidget: public RegistrationBaseWidget
{
Q_OBJECT
public:
	ElastixWidget(RegistrationManagerPtr regManager, QWidget* parent = NULL);
	virtual ~ElastixWidget();
	virtual QString defaultWhatsThis() const;

public slots:

private slots:
	void registerSlot();
	void userParameterFileSelected(QString filename);
	void browseExecutableSlot();
	void elastixChangedSlot();
	void executableEditFinishedSlot();
	void toggleDetailsSlot();
	void savePresetSlot();
	void deletePresetSlot();

private:
	QWidget* createOptionsWidget();

	QPushButton* mRegisterButton;
	ElastixManagerPtr mElastixManager;

	StringDataAdapterPtr mFixedImage;
	StringDataAdapterPtr mMovingImage;
	FileSelectWidget* mParameterFileWidget0;
	FilePreviewWidget* mFilePreviewWidget;
	QLineEdit* mExecutableEdit;
	cx::TimedAlgorithmProgressBar* mTimedAlgorithmProgressBar;
	//  QPushButton* mLaunchServerButton;
	QWidget* mOptionsWidget;
};

/**
 * @}
 */
}//namespace cx


#endif /* CXELASTIXWIDGET_H_ */
