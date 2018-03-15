/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXPLUGINFRAMEWORKWIDGET_H_
#define CXPLUGINFRAMEWORKWIDGET_H_

#include "cxGuiExport.h"

#include "cxBaseWidget.h"

namespace cx
{

/**
 * Widget for displaying and manipulating plugins
 *
 * \ingroup cx_gui
 *
 * \date 2014-04-30
 * \author Christian Askeland
 */
class cxGui_EXPORT PluginFrameworkWidget : public BaseWidget
{
	Q_OBJECT
public:
	PluginFrameworkWidget(QWidget* parent);
	virtual ~PluginFrameworkWidget();

protected:
	QVBoxLayout*  mVerticalLayout; ///< vertical layout is used
};

} /* namespace cx */

#endif /* CXPLUGINFRAMEWORKWIDGET_H_ */
