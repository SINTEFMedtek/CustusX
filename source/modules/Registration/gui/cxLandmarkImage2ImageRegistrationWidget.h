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
#ifndef CXLANDMARKIMAGE2IMAGEREGISTRATIONWIDGET_H_
#define CXLANDMARKIMAGE2IMAGEREGISTRATIONWIDGET_H_

#include "cxLandmarkRegistrationWidget.h"
#include "cxRegistrationDataAdapters.h"

class QVBoxLayout;
class QComboBox;
class QTableWidget;
class QPushButton;
class QLabel;
class QSlider;

namespace cx
{
/**
 * \file
 * \addtogroup cx_module_registration
 * @{
 */

/**
 * Widget for performing image to image registration
 * using landmarks.
 */
class LandmarkImage2ImageRegistrationWidget: public LandmarkRegistrationWidget
{
Q_OBJECT

public:

	LandmarkImage2ImageRegistrationWidget(RegistrationServicePtr registrationService, PatientModelServicePtr patientModelService, QWidget* parent, QString objectName,
		QString windowTitle); ///< sets up layout and connects signals and slots
	virtual ~LandmarkImage2ImageRegistrationWidget(); ///< empty
	virtual QString defaultWhatsThis() const;

protected slots:
	void updateRep();
	void registerSlot();
	void translationCheckBoxChanged();

protected:
	virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
	virtual void hideEvent(QHideEvent* event);
	virtual LandmarkMap getTargetLandmarks() const;
	virtual void performRegistration();
    virtual void prePaintEvent(); ///< populates the table widget
    virtual Transform3D getTargetTransform() const;
	virtual void setTargetLandmark(QString uid, Vector3D p_target);
	virtual QString getTargetName() const;

	ImageLandmarksSourcePtr mFixedLandmarkSource;
	ImageLandmarksSourcePtr mMovingLandmarkSource;

	//gui
	RegistrationFixedImageStringDataAdapterPtr mFixedDataAdapter;
	RegistrationMovingImageStringDataAdapterPtr mMovingDataAdapter;

	QPushButton* mRegisterButton;
	QCheckBox* mTranslationCheckBox;

private:
	LandmarkImage2ImageRegistrationWidget(); ///< not implemented
};

/**
 * @}
 */
}//namespace cx

#endif /* CXLANDMARKIMAGE2IMAGEREGISTRATIONWIDGET_H_ */
