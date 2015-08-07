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


namespace cx
{
typedef boost::shared_ptr<class StringPropertySelectMesh> StringPropertySelectMeshPtr;

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
	MeshInfoWidget(PatientModelServicePtr patientModelService, VisualizationServicePtr visualizationService, QWidget* parent);
  virtual ~MeshInfoWidget();

protected slots:
  void setColorSlot();
  void setColorSlotDelayed();
  void meshSelectedSlot();
  void importTransformSlot();
  void meshChangedSlot();

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QCloseEvent* event); ///<disconnects stuff

private:
	void addWidgets(cx::PatientModelServicePtr patientModelService);

  MeshPtr mMesh;
  StringPropertyParentFramePtr mParentFrameAdapter;
  StringPropertyDataNameEditablePtr mNameAdapter;
  StringPropertyDataUidEditablePtr mUidAdapter;
  StringPropertySelectMeshPtr mSelectMeshWidget;
  ColorPropertyPtr mColorAdapter;
  QCheckBox* mBackfaceCullingCheckBox;
  QCheckBox* mFrontfaceCullingCheckBox;
  DoublePropertyPtr mVisSizeWidget;
  QCheckBox* mGlyphVisualizationCheckBox;
  StringPropertyGlyphOrientationArrayPtr mGlyphOrientationArrayAdapter;
  StringPropertyGlyphColorArrayPtr mGlyphColorArrayAdapter;
  StringPropertyGlyphLUTPtr mGlyphColorLUTAdapter;


  PatientModelServicePtr mPatientModelService;
  VisualizationServicePtr mVisualizationService;
  
  MeshInfoWidget();
};

}//end namespace cx

#endif /* CXMESHINFOWIDGET_H_ */
