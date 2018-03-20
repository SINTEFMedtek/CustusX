/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
