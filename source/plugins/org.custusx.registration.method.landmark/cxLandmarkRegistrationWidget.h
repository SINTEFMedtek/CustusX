/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXLANDMARKREGISTRATIONWIDGET_H_
#define CXLANDMARKREGISTRATIONWIDGET_H_

#include "org_custusx_registration_method_landmarkExport.h"

#include <map>
#include "cxTransform3D.h"
#include "cxRegistrationBaseWidget.h"


class QVBoxLayout;
class QComboBox;
class QTableWidget;
class QPushButton;
class QLabel;
class QSlider;

namespace cx
{
typedef std::map<QString, class Landmark> LandmarkMap;
typedef boost::shared_ptr<class RegistrationManager> RegistrationManagerPtr;
typedef boost::shared_ptr<class LandmarkListener> LandmarkListenerPtr;

/**
 * \file
 * \addtogroup org_custusx_registration_method_landmark
 * @{
 */

/**
 * \class LandmarkRegistrationWidget
 * Superclass for Image Registration and Patient Registration
 */
class org_custusx_registration_method_landmark_EXPORT LandmarkRegistrationWidget: public RegistrationBaseWidget
{
Q_OBJECT

public:
	LandmarkRegistrationWidget(RegServicesPtr services, QWidget* parent, QString objectName,
        QString windowTitle, bool showAccuracy = true);
	virtual ~LandmarkRegistrationWidget();

protected slots:
	virtual void cellClickedSlot(int row, int column); ///< when a landmark is selected from the table

	void cellChangedSlot(int row, int column); ///< reacts when the user types in a (landmark) name
	void landmarkUpdatedSlot();
	void updateAverageAccuracyLabel();

protected:
	virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
	virtual void hideEvent(QHideEvent* event);
    virtual void prePaintEvent(); ///< populates the table widget
	virtual LandmarkMap getTargetLandmarks() const = 0;
	virtual void performRegistration() = 0;
	virtual Transform3D getTargetTransform() const = 0; ///< Return transform from target space to reference space
	virtual void setTargetLandmark(QString uid, Vector3D p_target) = 0;
	virtual QString getTargetName() const = 0;
	void setManualToolPosition(Vector3D p_r);
    QString getNextLandmark();
    void activateLandmark(QString uid);

	std::vector<Landmark> getAllLandmarks() const; ///< get all the landmarks from the image and the datamanager
	QString getLandmarkName(QString uid);
	double getAccuracy(QString uid);
	double getAverageAccuracy();

	//gui
	QVBoxLayout* mVerticalLayout; ///< vertical layout is used
	QTableWidget* mLandmarkTableWidget; ///< the table widget presenting the landmarks
	QLabel* mAvarageAccuracyLabel; ///< label showing the average accuracy

	//data
	QString mActiveLandmark; ///< uid of surrently selected landmark.
	LandmarkListenerPtr mLandmarkListener;
    bool mShowAccuracy;

private:
	LandmarkRegistrationWidget(); ///< not implemented
	bool isAverageAccuracyValid();
	double getAverageAccuracy(int &numActiveLandmarks);

};

/**
 * @}
 */
}//namespace cx


#endif /* CXLANDMARKREGISTRATIONWIDGET_H_ */
