/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXMESHGLYPHSWIDGET_H
#define CXMESHGLYPHSWIDGET_H

#include "cxGuiExport.h"

#include <vector>
#include <QtWidgets>

#include "cxMesh.h"
#include "cxDataInterface.h"
#include "cxBaseWidget.h"


namespace cx
{
typedef boost::shared_ptr<class SelectDataStringPropertyBase> SelectDataStringPropertyBasePtr;

/**
 * \class MeshGlyphsWidget
 *
 * \brief Widget for displaying glyps information about meshes.
 *
 * \ingroup cx_gui
 *
 * split from class MeshInfoWidget
 */
class cxGui_EXPORT MeshGlyphsWidget : public BaseWidget
{
  Q_OBJECT

public:
	MeshGlyphsWidget(SelectDataStringPropertyBasePtr meshSelector,
				   PatientModelServicePtr patientModelService, ViewServicePtr viewService,
				   QWidget* parent);
  virtual ~MeshGlyphsWidget();

	SelectDataStringPropertyBasePtr getSelector() { return mMeshSelector; }

protected slots:
  void meshSelectedSlot();
  void meshChangedSlot();

private:
	void addWidgets();

  MeshPtr mMesh;
  SelectDataStringPropertyBasePtr mMeshSelector;

  QCheckBox* mGlyphVisualizationCheckBox;
  StringPropertyGlyphOrientationArrayPtr mGlyphOrientationArrayAdapter;
  StringPropertyGlyphColorArrayPtr mGlyphColorArrayAdapter;
  StringPropertyGlyphLUTPtr mGlyphColorLUTAdapter;

  PatientModelServicePtr mPatientModelService;
  ViewServicePtr mViewService;

  MeshGlyphsWidget();
};

}//end namespace cx

#endif // CXMESHGLYPHSWIDGET_H
