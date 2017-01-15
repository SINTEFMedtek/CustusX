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
