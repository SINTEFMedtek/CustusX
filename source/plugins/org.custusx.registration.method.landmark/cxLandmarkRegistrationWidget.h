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
#ifndef CXLANDMARKREGISTRATIONWIDGET_H_
#define CXLANDMARKREGISTRATIONWIDGET_H_

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
class LandmarkRegistrationWidget: public RegistrationBaseWidget
{
Q_OBJECT

public:
	LandmarkRegistrationWidget(RegServicesPtr services, QWidget* parent, QString objectName,
		QString windowTitle);
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
