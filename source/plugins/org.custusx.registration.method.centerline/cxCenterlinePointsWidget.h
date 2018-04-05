/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

//TODO: Refactor this class to use ImageLandmarksWidget instead of duplicating code
/**
 * \class CenterlinePointsWidget
 *
 * \brief Widget for creating a centerline.vtk file from image landmarks.
 *
 * \date 2017-MAR-14
 * \\author Erlend F Hofstad, SINTEF
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
