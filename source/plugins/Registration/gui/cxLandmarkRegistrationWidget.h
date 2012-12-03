/**
 *\class LandmarkRegistrationWidget
 *
 *\brief
 *
 *  \date Apr 21, 2010
 *      \author dev
 */

#ifndef CXLANDMARKREGISTRATIONWIDGET_H_
#define CXLANDMARKREGISTRATIONWIDGET_H_

#include <map>
#include "sscVolumetricRep.h"
#include "sscLandmark.h"
#include "sscTransform3D.h"
#include "cxRegistrationBaseWidget.h"

class QVBoxLayout;
class QComboBox;
class QTableWidget;
class QPushButton;
class QLabel;
class QSlider;

namespace cx
{
typedef boost::shared_ptr<class RegistrationManager> RegistrationManagerPtr;

/**
 * \file
 * \addtogroup cxPluginRegistration
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
	LandmarkRegistrationWidget(RegistrationManagerPtr regManager, QWidget* parent, QString objectName,
		QString windowTitle);
	virtual ~LandmarkRegistrationWidget();

	virtual QString defaultWhatsThis() const;

protected slots:
	virtual void activeImageChangedSlot(); ///< listens to the datamanager for when the active image is changed
	virtual void cellClickedSlot(int row, int column); ///< when a landmark i selected from the table

	void cellChangedSlot(int row, int column); ///< reacts when the user types in a (landmark) name
	void landmarkUpdatedSlot();
	void updateAvarageAccuracyLabel();

protected:
	virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
	virtual void hideEvent(QHideEvent* event);
    virtual void prePaintEvent(); ///< populates the table widget
	virtual ssc::LandmarkMap getTargetLandmarks() const = 0;
	virtual void performRegistration() = 0;
	virtual ssc::Transform3D getTargetTransform() const = 0; ///< Return transform from target space to reference space
	virtual void setTargetLandmark(QString uid, ssc::Vector3D p_target) = 0;
	virtual QString getTargetName() const = 0;
	void setManualToolPosition(ssc::Vector3D p_r);
    QString getNextLandmark();
    void activateLandmark(QString uid);

//	void nextRow(); ///< jump to the next line in the tablewidget, updates active landmark and highlighted row
	std::vector<ssc::Landmark> getAllLandmarks() const; ///< get all the landmarks from the image and the datamanager
	QString getLandmarkName(QString uid);
	double getAccuracy(QString uid);
	double getAvarageAccuracy();

	//gui
	QVBoxLayout* mVerticalLayout; ///< vertical layout is used
	QTableWidget* mLandmarkTableWidget; ///< the table widget presenting the landmarks
	QLabel* mAvarageAccuracyLabel; ///< label showing the average accuracy

	//data
	QString mActiveLandmark; ///< uid of surrently selected landmark.

private:
	LandmarkRegistrationWidget(); ///< not implemented
	ssc::ImagePtr mCurrentImage; ///< the image currently used in image registration
};

/**
 * @}
 */
}//namespace cx


#endif /* CXLANDMARKREGISTRATIONWIDGET_H_ */
