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

#include "cxFilterPresetWidget.h"

#include "sscMessageManager.h"
#include "cxFilter.h"

namespace cx {

FilterPresetWidget::FilterPresetWidget(QWidget* parent) :
		PresetWidget(parent)
{}

void FilterPresetWidget::setFilter(FilterPtr filter)
{
	if(!filter->hasPresets()){
		ssc::messageManager()->sendError("Cannot use filter "+filter->getName()+" in the FilterPresetWidget because it does not have any presets.");
		return;
	}

	mFilter = filter;
	this->setPresets(mFilter->getPresets());
}

void FilterPresetWidget::saveSlot()
{
	if(!mFilter)
		return;

	QDomElement element = mFilter->getNewPreset();

	//DEBUG
	/*
	QDomNode n = element.firstChild();
	while(!n.isNull())  {
	    QDomElement e = n.toElement(); // try to convert the node to an element.
	    if(!e.isNull())  {
	        std::cout << qPrintable(e.tagName()) << std::endl; // the node really is an element.
	    }
	    n = n.nextSibling();
	}
	*/
	//DEBUG


	QString text = PresetWidget::getNewPresetName(true);
	mPresets->addCustomPreset(text, element);

	PresetWidget::saveSlot();
	PresetWidget::requestSetCurrentPreset(text);
}

} /* namespace cx */
