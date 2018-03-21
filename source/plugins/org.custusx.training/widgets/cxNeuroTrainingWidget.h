/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXNEUROTRAININGWIDGET_H
#define CXNEUROTRAININGWIDGET_H

#include "cxTrainingWidget.h"

namespace cx
{

class NeuroTrainingWidget : public TrainingWidget
{
	Q_OBJECT
	void startTracking();
	void changeWorkflowToRegistration();
	void changeWorkflowToUSAcquisition();
	void changeWorkflowToImport();
	void changeWorkflowToNavigation();
public:
	explicit NeuroTrainingWidget(RegServicesPtr services, ctkPluginContext *context, QWidget* parent = NULL);

    void onImport();
	void onRegisterStep();
	void onUse2DUSStep();
	void on3DUSAcqStep();
	void changeImageToPatientRegistrationToFast();
	void onShowAllUSStep();

private:
	ctkPluginContext* mPluginContext;
	void setUSSimulatorInput(QString usUid);
	void setSurfaceForPointCloudRegistration(QString uidPart);
};

} // cx
#endif // CXNEUROTRAININGWIDGET_H
