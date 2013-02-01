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

#include "cxFilter.h"

namespace cx
{

Filter::Filter() :
    QObject(NULL)
{
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

FilterGroup::FilterGroup(ssc::XmlOptionFile options) :
    mOptions(options)
{
}

/**
  * Get the option node for this pipeline
  */
ssc::XmlOptionFile FilterGroup::getOptions()
{
	return mOptions;
}

/**
  * Get all filters in pipeline
  */
std::vector<FilterPtr> FilterGroup::getFilters() const
{
	return mFilters;
}

void FilterGroup::append(FilterPtr filter)
{
	mFilters.push_back(filter);

	QString uid = QString("%1_%2").arg(filter->getType()).arg(mFilters.size());
	ssc::XmlOptionFile node = mOptions.descend(uid);
	filter->initialize(node.getElement(), uid);
}


} // namespace cx
