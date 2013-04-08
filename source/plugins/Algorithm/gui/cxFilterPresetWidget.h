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

#ifndef CXFILTERPRESETWIDGET_H_
#define CXFILTERPRESETWIDGET_H_

#include "cxPresetWidget.h"

#include "cxForwardDeclarations.h"

namespace cx {

/**
 * \class FilterPresetWidget
 *
 * \brief
 *
 * \ingroup cxPluginAlgorithm
 *
 * \date Mar 18, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class FilterPresetWidget : public PresetWidget
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
