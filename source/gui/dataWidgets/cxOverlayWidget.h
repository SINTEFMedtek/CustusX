/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXOVERLAYWIDGET_H_
#define CXOVERLAYWIDGET_H_

#include "cxGuiExport.h"

#include "cxBaseWidget.h"

namespace cx {
/**
 * \file
 * \addtogroup cx_gui
 * @{
 */

/*
 * \class OverlayWidget
 *
 *  \data Jul 18, 2013
 *  \author Ole Vegard Solberg, SINTEF
 */
class cxGui_EXPORT OverlayWidget : public BaseWidget
{
	Q_OBJECT

public:
	OverlayWidget(PatientModelServicePtr patientModelService, ViewServicePtr viewService, QWidget* parent);
};

/**
 * @}
 */
} /* namespace cx */
#endif /* CXOVERLAYWIDGET_H_ */
