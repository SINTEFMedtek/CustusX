/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXFILEPATHPROPERTY_H
#define CXFILEPATHPROPERTY_H

#include "cxResourceExport.h"

#include <QStringList>
#include <boost/shared_ptr.hpp>
#include "cxFilePathPropertyBase.h"

namespace cx
{

typedef boost::shared_ptr<class FilePathProperty> FilePathPropertyPtr;

/**
 * \ingroup cx_resource_core_properties
 */
class cxResource_EXPORT FilePathProperty: public FilePathPropertyBase
{
	Q_OBJECT
public:
	/** Make sure one given option exists witin root.
		 * If not present, fill inn the input defaults.
		 */
	static FilePathPropertyPtr initialize(const QString& uid, QString name, QString help, QString value,
																				QStringList paths, QDomNode root = QDomNode());

public:
	// inherited interface
	virtual QString getUid() const;

	virtual QVariant getValueAsVariant() const;
	virtual void setValueFromVariant(QVariant val);

	virtual QString getHelp() const; ///< return a descriptive help string for the data, used for example as a tool tip.
	virtual void setHelp(QString val);

protected:
	FilePathProperty();

};


} // namespace cx

#endif // CXFILEPATHPROPERTY_H
