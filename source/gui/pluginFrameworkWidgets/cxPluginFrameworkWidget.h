// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXPLUGINFRAMEWORKWIDGET_H_
#define CXPLUGINFRAMEWORKWIDGET_H_

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
class PluginFrameworkWidget : public BaseWidget
{
	Q_OBJECT
public:
	PluginFrameworkWidget(QWidget* parent);
	virtual ~PluginFrameworkWidget();

	virtual QString defaultWhatsThis() const;

protected:
	QVBoxLayout*  mVerticalLayout; ///< vertical layout is used
};

} /* namespace cx */

#endif /* CXPLUGINFRAMEWORKWIDGET_H_ */
