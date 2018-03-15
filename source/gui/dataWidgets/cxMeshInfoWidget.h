/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXMESHINFOWIDGET_H_
#define CXMESHINFOWIDGET_H_

#include "cxGuiExport.h"

#include <vector>
#include <QtWidgets>

#include "cxMesh.h"
#include "cxDataInterface.h"
#include "cxInfoWidget.h"
#include "cxTabbedWidget.h"


namespace cx
{
typedef boost::shared_ptr<class SelectDataStringPropertyBase> SelectDataStringPropertyBasePtr;

class cxGui_EXPORT ActiveMeshPropertiesWidget : public BaseWidget
{
  Q_OBJECT
public:
	ActiveMeshPropertiesWidget(VisServicesPtr services, QWidget* parent);
  virtual ~ActiveMeshPropertiesWidget() {}
};

class cxGui_EXPORT AllMeshPropertiesWidget : public TabbedWidget
{
  Q_OBJECT
public:
	AllMeshPropertiesWidget(SelectDataStringPropertyBasePtr mesh, VisServicesPtr services, QWidget* parent);
  virtual ~AllMeshPropertiesWidget() {}
	SelectDataStringPropertyBasePtr getSelector() { return mMeshSelector; }
private:
	SelectDataStringPropertyBasePtr mMeshSelector;
};

/// -------------------------------------------------------

/**
 * \class MeshInfoWidget
 *
 * \brief Widget for displaying information about meshes.
 *
 * \ingroup cx_gui
 *
 * \date 2010.07.07
 * \author Ole Vegard Solberg, SINTEF
 * \author Janne Beate Bakeng, SINTEF
 */
class cxGui_EXPORT MeshInfoWidget : public InfoWidget
{
  Q_OBJECT

public:
	MeshInfoWidget(SelectDataStringPropertyBasePtr meshSelector,
				   PatientModelServicePtr patientModelService, ViewServicePtr viewService,
				   QWidget* parent);
  virtual ~MeshInfoWidget();

	SelectDataStringPropertyBasePtr getSelector() { return mMeshSelector; }

protected slots:
  void meshSelectedSlot();
  void importTransformSlot();
  void meshChangedSlot();
  void generateNormalsSlot();

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QCloseEvent* event); ///<disconnects stuff

private:
	void addWidgets();

  MeshPtr mMesh;
  StringPropertyParentFramePtr mParentFrameAdapter;
  StringPropertyDataNameEditablePtr mNameAdapter;
  StringPropertyDataUidEditablePtr mUidAdapter;
  SelectDataStringPropertyBasePtr mMeshSelector;

  PatientModelServicePtr mPatientModelService;
  ViewServicePtr mViewService;
  
  MeshInfoWidget();
};

}//end namespace cx

#endif /* CXMESHINFOWIDGET_H_ */
