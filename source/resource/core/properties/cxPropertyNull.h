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

#ifndef CXPROPERTYNULL_H_
#define CXPROPERTYNULL_H_

#include "cxResourceExport.h"
#include "cxProperty.h"

namespace cx {

/**
 * \ingroup cx_resource_core_properties
 * \author Christian Askeland, SINTEF
 * \date 2014-10-23
 *
 */
class cxResource_EXPORT PropertyNull: public Property
{
	Q_OBJECT
public:
	virtual ~PropertyNull(){}
	static PropertyPtr create() { return PropertyPtr(new PropertyNull()); }

public:
    virtual QString getDisplayName() const { return ""; }
    virtual QString getUid() const  { return ""; }

    virtual QVariant getValueAsVariant() const  { return QVariant(); }
    virtual void setValueFromVariant(QVariant val) {}

    virtual bool getEnabled() const { return false; }
    virtual bool getAdvanced() const { return false; }
    virtual QString getGroup() const { return ""; }

    virtual bool setEnabled(bool enabled) { return false;}
    virtual bool setAdvanced(bool advanced) { return false;}
    virtual bool setGroup(QString name) { return false;}
};

} //namespace cx


#endif // CXPROPERTYNULL_H_
