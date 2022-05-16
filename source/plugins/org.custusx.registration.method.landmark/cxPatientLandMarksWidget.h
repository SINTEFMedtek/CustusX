/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXPATIENTLANDMARKSWIDGET_H
#define CXPATIENTLANDMARKSWIDGET_H

#include "cxLandmarkRegistrationWidget.h"
#include "org_custusx_registration_method_landmark_Export.h"

namespace cx
{

typedef boost::shared_ptr<class ActiveToolProxy> ActiveToolProxyPtr;

/**
 * \class PatientLandMarksWidget
 *
 * \brief Widget used as a tab in the ContexDockWidget for patient registration.
 *
 * \date Feb 3, 2009
 * \\author Janne Beate Bakeng, SINTEF
 */
class org_custusx_registration_method_landmark_EXPORT PatientLandMarksWidget: public LandmarkRegistrationWidget
{
Q_OBJECT

public:
	PatientLandMarksWidget(RegServicesPtr services, QWidget* parent, QString objectName,
		QString windowTitle); ///< sets up layout and connects signals and slots
	virtual ~PatientLandMarksWidget(); ///< empty

protected slots:

//	void registerSlot();
	void toolSampleButtonClickedSlot(); ///< reacts when the Sample Tool button is clicked
	virtual void cellClickedSlot(int row, int column); ///< when a landmark i selected from the table
	void removeLandmarkButtonClickedSlot();
	void updateToolSampleButton();
	virtual void pointSampled(Vector3D p_r);

protected:
	virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
	virtual void hideEvent(QHideEvent* event);
	virtual void prePaintEvent(); ///< populates the table widget
	virtual LandmarkMap getTargetLandmarks() const;
	virtual Transform3D getTargetTransform() const;
	virtual void setTargetLandmark(QString uid, Vector3D p_target);
	virtual QString getTargetName() const;
	virtual void performRegistration(); // no registration in this widget - only definition of pts.

	//gui
	QPushButton* mToolSampleButton; ///< the Sample Tool button
	QPushButton* mRemoveLandmarkButton;

	//data
	ActiveToolProxyPtr mActiveToolProxy;

private slots:
	void globalConfigurationFileChangedSlot(QString key);
private:
	PatientLandMarksWidget(); ///< not implemented
};

} //cx

#endif // CXPATIENTLANDMARKSWIDGET_H
