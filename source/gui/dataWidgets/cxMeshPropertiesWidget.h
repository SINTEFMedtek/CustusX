/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXMESHPROPERTIESWIDGET_H
#define CXMESHPROPERTIESWIDGET_H

#include "cxGuiExport.h"

#include <vector>
//#include <QtWidgets>

#include "cxMesh.h"
#include "cxDataInterface.h"
#include "cxBaseWidget.h"


namespace cx
{
typedef boost::shared_ptr<class SelectDataStringPropertyBase> SelectDataStringPropertyBasePtr;
class ReplacableContentWidget;

/**
 * \class MeshPropertiesWidget
 *
 * \brief Widget for displaying glyps information about meshes.
 *
 * \ingroup cx_gui
 *
 * split from class MeshInfoWidget
 */
class cxGui_EXPORT MeshPropertiesWidget : public BaseWidget
{
  Q_OBJECT

public:
	MeshPropertiesWidget(SelectDataStringPropertyBasePtr meshSelector,
				   PatientModelServicePtr patientModelService, ViewServicePtr viewService,
				   QWidget* parent);
  virtual ~MeshPropertiesWidget();

	SelectDataStringPropertyBasePtr getSelector() { return mMeshSelector; }

protected:
  void setupUI();
  virtual void prePaintEvent();

private:
  void updateFrontend();

protected slots:
  void meshSelectedSlot();

private:
  MeshPtr mMesh;
  SelectDataStringPropertyBasePtr mMeshSelector;

  ReplacableContentWidget* mPropertiesWidget;

  PatientModelServicePtr mPatientModelService;
  ViewServicePtr mViewService;

  MeshPropertiesWidget();
  void clearUI();
};

}//end namespace cx

#endif // CXMESHPROPERTIESWIDGET_H
