#ifndef CXLANDMARKPATIENTREGISTRATIONWIDGET_H_
#define CXLANDMARKPATIENTREGISTRATIONWIDGET_H_

#include "cxLandmarkRegistrationWidget.h"
#include "cxRegistrationDataAdapters.h"

#include "cxImage.h"
#include "cxTransform3D.h"
#include "cxDominantToolProxy.h"

class QVBoxLayout;
class QComboBox;
class QTableWidget;
class QPushButton;
class QString;
class QLabel;
class QSlider;
class QGridLayout;
class QSpinBox;

namespace cx
{
typedef Transform3D Transform3D;

/**
 * \file
 * \addtogroup cx_module_registration
 * @{
 */

/**
 * \class LandmarkPatientRegistrationWidget
 *
 * \brief Widget used as a tab in the ContexDockWidget for patient registration.
 *
 * \date Feb 3, 2009
 * \\author Janne Beate Bakeng, SINTEF
 */
class LandmarkPatientRegistrationWidget: public LandmarkRegistrationWidget
{
Q_OBJECT

public:
	LandmarkPatientRegistrationWidget(RegistrationManagerPtr regManager, QWidget* parent, QString objectName,
		QString windowTitle); ///< sets up layout and connects signals and slots
	virtual ~LandmarkPatientRegistrationWidget(); ///< empty
	virtual QString defaultWhatsThis() const;

protected slots:

	void registerSlot();
	virtual void fixedDataChanged(); ///< listens to the datamanager for when the active image is changed
	void toolSampleButtonClickedSlot(); ///< reacts when the Sample Tool button is clicked
	virtual void cellClickedSlot(int row, int column); ///< when a landmark i selected from the table
	void removeLandmarkButtonClickedSlot();
	void updateToolSampleButton();

protected:
	virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
	virtual void hideEvent(QHideEvent* event);
    virtual void prePaintEvent(); ///< populates the table widget
    virtual LandmarkMap getTargetLandmarks() const;
	virtual Transform3D getTargetTransform() const;
	virtual void setTargetLandmark(QString uid, Vector3D p_target);
	virtual QString getTargetName() const;
	virtual void performRegistration();

	//gui
	QPushButton* mToolSampleButton; ///< the Sample Tool button
	QPushButton* mRemoveLandmarkButton;
	QPushButton* mRegisterButton;

	//data
	ImageLandmarksSourcePtr mImageLandmarkSource;
	RegistrationFixedImageStringDataAdapterPtr mFixedDataAdapter;
	DominantToolProxyPtr mDominantToolProxy;

private:
	LandmarkPatientRegistrationWidget(); ///< not implemented
};

/**
 * @}
 */
}//namespace cx

#endif /* CXLANDMARKPATIENTREGISTRATIONWIDGET_H_ */
