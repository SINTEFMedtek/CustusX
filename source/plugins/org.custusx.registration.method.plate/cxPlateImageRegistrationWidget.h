#ifndef CXPLATEIMAGEREGISTRATIONWIDGET_H
#define CXPLATEIMAGEREGISTRATIONWIDGET_H

#include "cxFastImageRegistrationWidget.h"

namespace cx
{

/**
 * Widget for performing plate (landmark based) image registration using only the
 * translation part of the matrix.
 *
 * \ingroup org_custusx_registration_method_Plate
 *
 * \date 2014-10-09
 * \author Ole Vegard Solberg, SINTEF
 * \author Janne Beate Bakeng, SINTEF
 */
class PlateImageRegistrationWidget : public FastImageRegistrationWidget
{
//	Q_OBJECT
public:
	PlateImageRegistrationWidget(RegistrationServicePtr registrationService, PatientModelServicePtr patientModelService, QWidget* parent);
	virtual ~PlateImageRegistrationWidget();
	virtual QString defaultWhatsThis() const;

protected slots:
	virtual void editLandmarkButtonClickedSlot();

protected:
	virtual void performRegistration();

	PatientModelServicePtr mPatientModelService;
};

} //cx

#endif // CXPLATEIMAGEREGISTRATIONWIDGET_H
