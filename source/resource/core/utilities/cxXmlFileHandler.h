/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXXMLFILEHANDLER_H
#define CXXMLFILEHANDLER_H

#include "cxResourceExport.h"

class QDomDocument;


namespace cx
{

/**\brief Helper class for reading and writing an XML file.
 *
 * \ingroup cx_resource_core_utilities
 */
class cxResource_EXPORT XmlFileHandler
{
public:
    static QDomDocument readXmlFile(QString& filename);
    static void writeXmlFile(QDomDocument& doc, QString& filename);

};

} // namespace cx

#endif // CXXMLFILEHANDLER_H
