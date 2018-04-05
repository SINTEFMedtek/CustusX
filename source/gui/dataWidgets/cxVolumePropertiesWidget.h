/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVOLUMEPROPERTIESWIDGET_H_
#define CXVOLUMEPROPERTIESWIDGET_H_

#include "cxGuiExport.h"

#include "cxTabbedWidget.h"
#include "cxForwardDeclarations.h"
#include "cxDataInterface.h"

namespace cx
{
/**
 * \class ActiveVolumeWidget
 * \brief Widget that contains a select active image combobox.
 * \ingroup cx_gui
 * \date Aug 20, 2010
 * \author Christian Askeland, SINTEF
 */
class cxGui_EXPORT ActiveVolumeWidget : public BaseWidget
{
  Q_OBJECT
public:
	ActiveVolumeWidget(PatientModelServicePtr patientModelService, ViewServicePtr viewService, QWidget* parent);
  ~ActiveVolumeWidget() {}

};

/**
 * \class VolumePropertiesWidget
 * \brief Widget for displaying and manipulating various 3D Volume properties.
 * \ingroup cx_gui
 *
 * \date Aug 20, 2010
 * \author christiana
 */
class cxGui_EXPORT VolumePropertiesWidget : public TabbedWidget
{
  Q_OBJECT
public:
	VolumePropertiesWidget(VisServicesPtr services, QWidget* parent);
  virtual ~VolumePropertiesWidget() {}
};

}//namespace cx


#endif /* CXVOLUMEPROPERTIESWIDGET_H_ */
