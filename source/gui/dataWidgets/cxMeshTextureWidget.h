/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/#ifndef MESHTEXTUREWIDGET_H
#define MESHTEXTUREWIDGET_H

#include "cxGuiExport.h"
#include "cxBaseWidget.h"
#include "cxOptionsWidget.h"


namespace cx
{

class cxGui_EXPORT MeshTextureWidget : public BaseWidget
{
	Q_OBJECT

public:
	MeshTextureWidget(SelectDataStringPropertyBasePtr meshSelector,
					  PatientModelServicePtr patientModelService, ViewServicePtr viewService,
					  QWidget *parent);

	virtual ~MeshTextureWidget();
	SelectDataStringPropertyBasePtr getSelector() { return mMeshSelector; }

protected slots:
	void setupUI();
	virtual void prePaintEvent();
	void meshSelectedSlot();

private:
	MeshTextureWidget();
	MeshPtr mMesh;
	SelectDataStringPropertyBasePtr mMeshSelector;
	OptionsWidget* mOptionsWidget;
	PatientModelServicePtr mPatientModelService;
	ViewServicePtr mViewService;
	void clearUI();
};

}//end namespace cx

#endif // MESHTEXTUREWIDGET_H
