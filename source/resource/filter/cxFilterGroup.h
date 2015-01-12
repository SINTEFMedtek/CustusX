/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
