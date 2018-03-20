/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXCOLORWIDGET_H_
#define CXCOLORWIDGET_H_

#include "cxGuiExport.h"

#include "cxBaseWidget.h"

namespace cx
{
typedef boost::shared_ptr<class ActiveData> ActiveDataPtr;

/**
 * \file
 * \addtogroup cx_gui
 * @{
 */

/*
 * \class ColorWidget
 *
 *  \data Jul 18, 2013
 *  \author Ole Vegard Solberg, SINTEF
 */
class cxGui_EXPORT ColorWidget : public BaseWidget
{
	Q_OBJECT

public:
	ColorWidget(PatientModelServicePtr patientModelService, QWidget* parent);
};

/**
 * @}
 */
} /* namespace cx */
#endif /* CXCOLORWIDGET_H_ */
