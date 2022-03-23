/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXLANDMARKIMAGE2IMAGEREGISTRATIONWIDGET_H_
#define CXLANDMARKIMAGE2IMAGEREGISTRATIONWIDGET_H_

#include "cxLandmarkRegistrationWidget.h"

class QVBoxLayout;
class QComboBox;
class QTableWidget;
class QPushButton;
class QLabel;
class QSlider;

namespace cx
{
typedef boost::shared_ptr<class ImageLandmarksSource> ImageLandmarksSourcePtr;
typedef boost::shared_ptr<class StringPropertyRegistrationFixedImage> StringPropertyRegistrationFixedImagePtr;
typedef boost::shared_ptr<class StringPropertyRegistrationMovingImage> StringPropertyRegistrationMovingImagePtr;

/**
 * \file
 * \addtogroup org_custusx_registration_method_landmark
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

	LandmarkImage2ImageRegistrationWidget(RegServicesPtr services, QWidget* parent, QString objectName,
										  QString windowTitle); ///< sets up layout and connects signals and slots
	virtual ~LandmarkImage2ImageRegistrationWidget(); ///< empty
	virtual QString defaultWhatsThis() const;

protected slots:
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

	//gui
	StringPropertyRegistrationFixedImagePtr mFixedProperty;
	StringPropertyRegistrationMovingImagePtr mMovingProperty;

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
