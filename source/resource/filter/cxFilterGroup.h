/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXFILTERGROUP_H_
#define CXFILTERGROUP_H_

#include "cxResourceFilterExport.h"

#include <vector>
#include "cxFilter.h"
#include "cxXmlOptionItem.h"


namespace cx {


/** Collection of filters.
  *
  * Connects them by giving them unique id's.
  *
  */
class cxResourceFilter_EXPORT FilterGroup
{
public:
    FilterGroup(XmlOptionFile options);
    /**
      * Get the option node for this pipeline
      */
    XmlOptionFile getOptions();
    /**
      * Get all filters in pipeline
      */
    std::vector<FilterPtr> getFilters() const;
    /**
      * Append a filter to group
      */
    void append(FilterPtr filter);
    void remove(Filter* filter);

    size_t size() const { return mFilters.size(); }
    bool empty() const { return mFilters.empty(); }
    //    FilterPtr& operator[](size_t index) { return mFilters[index]; }
    //    const FilterPtr& operator[](size_t index) const { return mFilters[index]; }
    FilterPtr get(int index) { return mFilters[index]; }
    FilterPtr get(QString uid)
    {
        for (unsigned i=0; i<mFilters.size(); ++i)
            if (mFilters[i]->getUid()==uid)
                return mFilters[i];
        return FilterPtr();
    }

private:
    std::vector<FilterPtr> mFilters;
    XmlOptionFile mOptions;
};
typedef boost::shared_ptr<FilterGroup> FilterGroupPtr;

} //namespace cx

#endif /* CXFILTERGROUP_H_ */
