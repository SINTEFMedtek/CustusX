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

#ifndef CXLANDMARKIMAGEREGISTRATIONWIDGET_H_
#define CXLANDMARKIMAGEREGISTRATIONWIDGET_H_

#include "org_custusx_registration_method_centerline_Export.h"

#include "cxLandmarkRegistrationWidget.h"
#include "cxRegistrationProperties.h"
#include "cxActiveToolProxy.h"

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
 * \addtogroup org_custusx_registration_method_centerline
 * @{
 */

/**
 * \class LandmarkImageRegistrationWidget
 *
 * \brief Widget for adding image landmarks, used by landmark based image registration methods
 *
 * \date Jan 27, 2009
 * \\author Janne Beate Bakeng, SINTEF
 */
class org_custusx_registration_method_centerline_EXPORT CenterlinePointsWidget: public LandmarkRegistrationWidget
{
Q_OBJECT

public:

    CenterlinePointsWidget(RegServicesPtr services, QWidget* parent, QString objectName,
		QString windowTitle, bool useRegistrationFixedPropertyInsteadOfActiveImage = false); ///< sets up layout and connects signals and slots
    virtual ~CenterlinePointsWidget(); ///< empty

protected slots:
//	virtual void activeImageChangedSlot(); ///< listens to the datamanager for when the active image is changed
	void addLandmarkButtonClickedSlot(); ///< reacts when the Add Landmark button is clicked
	virtual void editLandmarkButtonClickedSlot(); ///< reacts when the Edit Landmark button is clicked
    void removeLandmarkButtonClickedSlot(); ///< reacts when the Remove Landmark button is clicked
    void createCenterlineButtonClickedSlot(); ///< reacts when the Create centerline button is clicked
	virtual void cellClickedSlot(int row, int column); ///< when a landmark is selected from the table
	void enableButtons();
	void onCurrentImageChanged();

protected:
	virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
	virtual void hideEvent(QHideEvent* event);
	virtual QString getTargetName() const;
	virtual LandmarkMap getTargetLandmarks() const;
	virtual void setTargetLandmark(QString uid, Vector3D p_target);
	virtual void performRegistration() // no registration in this widget - only definition of pts.
	{
	}
    virtual void prePaintEvent(); ///< populates the table widget
    QString getLandmarkName(QString uid);
	virtual Transform3D getTargetTransform() const;
	PickerRepPtr getPickerRep();

	//gui
	SelectDataStringPropertyBasePtr mCurrentProperty;
	ActiveToolProxyPtr mActiveToolProxy;

	QPushButton* mAddLandmarkButton; ///< the Add Landmark button
	QPushButton* mEditLandmarkButton; ///< the Edit Landmark button
    QPushButton* mRemoveLandmarkButton; ///< the Remove Landmark button
    QPushButton* mCreateCenterlineButton; ///< the Create centerline button

private:
    CenterlinePointsWidget(); ///< not implemented

	DataPtr getCurrentData() const;
    bool mUseRegistrationFixedPropertyInsteadOfActiveImage;
};

/**
 * @}
 */
}//namespace cx

#endif /* CXLANDMARKIMAGEREGISTRATIONWIDGET_H_ */
