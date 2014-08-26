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

#ifndef CXCOLORDATAADAPTER_H_
#define CXCOLORDATAADAPTER_H_

#include <QColor>
#include "cxDataAdapter.h"


namespace cx
{

/** DataAdapter interface for QColor
 *
 *
 * \ingroup cx_resource_core_dataadapters
 * \date Nov 22, 2012
 * \author christiana
 */
class ColorDataAdapter: public DataAdapter
{
Q_OBJECT
public:
    virtual ~ColorDataAdapter() {}

public:
    // basic methods
    virtual QString getDisplayName() const = 0; ///< name of data entity. Used for display to user.
    virtual bool setValue(QColor value) = 0; ///< set the data value.
    virtual QColor getValue() const = 0; ///< get the data value.

public:
    // optional methods
    virtual QString getHelp() const
    {
        return QString();
    } ///< return a descriptive help string for the data, used for example as a tool tip.
};
typedef boost::shared_ptr<ColorDataAdapter> ColorDataAdapterPtr;

} // namespace cx

#endif // CXCOLORDATAADAPTER_H_
