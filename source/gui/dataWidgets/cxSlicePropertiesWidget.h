/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXSlicePropertiesWidget_H_
#define CXSlicePropertiesWidget_H_

#include "cxGuiExport.h"

#include <vector>
#include <QtWidgets>

#include "cxTabbedWidget.h"

namespace cx
{

/**
 * \file
 * \addtogroup cx_gui
 * @{
 */

/**
 * \class SlicePropertiesWidget
 *
 * \date 2010.04.12
 * \author Christian Askeland, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 */
class cxGui_EXPORT SlicePropertiesWidget : public TabbedWidget
{
  Q_OBJECT

public:
	SlicePropertiesWidget(cx::PatientModelServicePtr patientModelService, cx::ViewServicePtr viewService, QWidget* parent);
	virtual ~SlicePropertiesWidget();

protected:
private:
	SlicePropertiesWidget();
};

/**
 * @}
 */
}//end namespace cx

#endif /* CXSlicePropertiesWidget_H_ */
