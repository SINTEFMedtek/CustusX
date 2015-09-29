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

#ifndef CXVOLUMEINFOWIDGET_H_
#define CXVOLUMEINFOWIDGET_H_

#include "cxGuiExport.h"

#include "cxInfoWidget.h"

#include "cxForwardDeclarations.h"
#include "cxDataInterface.h"

#include <QTableWidget>

namespace cx
{

/**\brief Widget for displaying various volume information.
 *
 * \ingroup cx_gui
 *
 * \author Christian Askeland, SINTEF
 * \author Janne Beate Bakeng, SINTEF
 */
class cxGui_EXPORT VolumeInfoWidget : public InfoWidget
{
  Q_OBJECT

public:
  VolumeInfoWidget(PatientModelServicePtr patientModelService, QWidget* parent);
  virtual ~VolumeInfoWidget();

protected slots:
  void updateSlot();

private:
  void addWidgets();

  StringPropertyParentFramePtr mParentFrameAdapter;
  StringPropertyDataNameEditablePtr mNameAdapter;
  StringPropertyDataUidEditablePtr mUidAdapter;
  StringPropertyDataModalityPtr mModalityAdapter;
  StringPropertyImageTypePtr mImageTypeAdapter;
  ActiveImageProxyPtr mActiveImageProxy;
  PatientModelServicePtr mPatientModelService;
  ActiveDataPtr mActiveData;
};

}//namespace cx



#endif /* CXVOLUMEINFOWIDGET_H_ */
