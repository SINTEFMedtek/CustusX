#include <cxFilterGroup.h>

namespace cx {

FilterGroup::FilterGroup(XmlOptionFile options) :
    mOptions(options)
{
}

/**
  * Get the option node for this pipeline
  */
XmlOptionFile FilterGroup::getOptions()
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
    XmlOptionFile node = mOptions.descend(uid);
    filter->initialize(node.getElement(), uid);
}

void FilterGroup::remove(Filter* filter)
{
    for (unsigned i=0; i<mFilters.size(); )
    {
        if (filter == mFilters[i].get())
            mFilters.erase(mFilters.begin()+i);
        else
            ++i;
    }
}

}//namespace cx
