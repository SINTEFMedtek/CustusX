/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxRMPCFromPointerService.h"

#include "cxRMPCFromPointerWidget.h"

namespace cx
{

QWidget *RMPCFromPointerImageToPatientService::createWidget()
{
	RMPCFromPointerWidget* widget = new RMPCFromPointerWidget(mServices, NULL);
	return widget;
}

} /* namespace cx */
