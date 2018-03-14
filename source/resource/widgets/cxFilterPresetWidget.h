/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXFILTERPRESETWIDGET_H_
#define CXFILTERPRESETWIDGET_H_

#include "cxResourceWidgetsExport.h"

#include "cxPresetWidget.h"

#include "cxForwardDeclarations.h"

namespace cx {

/**
 *
 * \ingroup cx_resource_widgets
 *
 * \date Mar 18, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class cxResourceWidgets_EXPORT FilterPresetWidget : public PresetWidget
{
	Q_OBJECT

public:
	FilterPresetWidget(QWidget* parent);
	virtual ~FilterPresetWidget(){};

	void setFilter(FilterPtr filter); ///< sets which filter to operate on

public slots:
	virtual void saveSlot(); ///< called when user tries to save a filter preset
	virtual void deleteSlot(); ///< called when user tries to delete a filter preset

private:
	FilterPtr mFilter;
};

} /* namespace cx */
#endif /* CXFILTERPRESETWIDGET_H_ */
