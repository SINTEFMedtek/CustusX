/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRegistrationMethodCommandLineService.h"

#include "cxElastixWidget.h"

namespace cx
{

QWidget *RegistrationMethodCommandLineService::createWidget()
{
	ElastixWidget* widget = new ElastixWidget(mServices, NULL);
	return widget;
}



} /* namespace cx */
