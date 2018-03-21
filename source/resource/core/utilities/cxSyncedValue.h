/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXSYNCEDVALUE_H
#define CXSYNCEDVALUE_H

#include "cxResourceExport.h"

#include <QVariant>
#include <QObject>
#include "boost/shared_ptr.hpp"

namespace cx
{

typedef boost::shared_ptr<class SyncedValue> SyncedValuePtr;

/** A value intended for sharing between several objects
 *
 *  Use the changed() signal to listen for changes made by others.
 */
class cxResource_EXPORT  SyncedValue: public QObject
{
Q_OBJECT
public:
	SyncedValue(QVariant val = QVariant());
	static SyncedValuePtr create(QVariant val = QVariant());
	void set(QVariant val);
	QVariant get() const;
	template<class T>
	T value() const { return this->get().value<T>(); }
private:
	QVariant mValue;
signals:
	void changed();
};

} //namespace cx

#endif // CXSYNCEDVALUE_H
