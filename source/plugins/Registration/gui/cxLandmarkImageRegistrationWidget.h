#ifndef CXLANDMARKIMAGEREGISTRATIONWIDGET_H_
#define CXLANDMARKIMAGEREGISTRATIONWIDGET_H_

#include "cxLandmarkRegistrationWidget.h"
#include "cxRegistrationDataAdapters.h"
#include "Rep/cxLandmarkRep.h"
#include "cxDominantToolProxy.h"

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
 * \addtogroup cxPluginRegistration
 * @{
 */

/**
 * \class LandmarkImageRegistrationWidget
 *
 * \brief Widget for performing landmark based image registration
 *
 * \date Jan 27, 2009
 * \\author Janne Beate Bakeng, SINTEF
 */
class LandmarkImageRegistrationWidget: public LandmarkRegistrationWidget
{
Q_OBJECT

public:

	LandmarkImageRegistrationWidget(RegistrationManagerPtr regManager, QWidget* parent, QString objectName,
		QString windowTitle); ///< sets up layout and connects signals and slots
	virtual ~LandmarkImageRegistrationWidget(); ///< empty
	virtual QString defaultWhatsThis() const;

protected slots:
	virtual void activeImageChangedSlot(); ///< listens to the datamanager for when the active image is changed
	void addLandmarkButtonClickedSlot(); ///< reacts when the Add Landmark button is clicked
	virtual void editLandmarkButtonClickedSlot(); ///< reacts when the Edit Landmark button is clicked
	void removeLandmarkButtonClickedSlot(); ///< reacts when the Remove Landmark button is clicked
	virtual void cellClickedSlot(int row, int column); ///< when a landmark i selected from the table
	void enableButtons();

protected:
	virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
	virtual void hideEvent(QHideEvent* event);
	virtual QString getTargetName() const;
	virtual ssc::LandmarkMap getTargetLandmarks() const;
	virtual void setTargetLandmark(QString uid, ssc::Vector3D p_target);
	virtual void performRegistration() // no registration in this widget - only definition of pts.
	{
	}
    virtual void prePaintEvent(); ///< populates the table widget
    QString getLandmarkName(QString uid);
	virtual ssc::Transform3D getTargetTransform() const;
	ssc::PickerRepPtr getPickerRep();

	//gui
	ssc::StringDataAdapterPtr mActiveImageAdapter;
	ImageLandmarksSourcePtr mImageLandmarkSource;
	DominantToolProxyPtr mDominantToolProxy;

	QPushButton* mAddLandmarkButton; ///< the Add Landmark button
	QPushButton* mEditLandmarkButton; ///< the Edit Landmark button
	QPushButton* mRemoveLandmarkButton; ///< the Remove Landmark button

private:
	LandmarkImageRegistrationWidget(); ///< not implemented
};

/**
 * @}
 */
}//namespace cx

#endif /* CXLANDMARKIMAGEREGISTRATIONWIDGET_H_ */
