/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXFILEPREVIEWPROPERTY_H
#define CXFILEPREVIEWPROPERTY_H

//#include "cxStringPropertyBase.h"
#include "cxFilePathPropertyBase.h"

namespace cx
{

typedef boost::shared_ptr<class FilePreviewProperty> FilePreviewPropertyPtr;

/**
 * \class FilePreviewProperty
 *
 * \brief Property that connects to a file. Allows file preview and edit.
 *
 * \date Apr 23, 2020
 * \author Ole Vegard Solberg, SINTEF
 */
class cxResource_EXPORT FilePreviewProperty: public FilePathPropertyBase
{
	Q_OBJECT
public:
	/** Make sure one given option exists witin root.
	 * If not present, fill inn the input defaults.
	 */
	static FilePreviewPropertyPtr initialize(const QString& uid, QString name, QString help, QString value,
																					 QStringList paths, QDomNode root = QDomNode());

protected:
	FilePreviewProperty();
};

}//cx

#endif // CXFILEPREVIEWPROPERTY_H
