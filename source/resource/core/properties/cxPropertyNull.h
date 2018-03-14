/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
