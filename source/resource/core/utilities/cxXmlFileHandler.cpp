/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxXmlFileHandler.h"
#include "cxLogger.h"
#include <QFile>
#include <QTextStream>


namespace cx
{

QDomDocument XmlFileHandler::readXmlFile(QString& filename)
{
    QDomDocument retval;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        reportError("Could not open XML file :" + file.fileName() + ".");
        return QDomDocument();
    }

    QString emsg;
    int eline, ecolumn;
    // Read the file
    if (!retval.setContent(&file, false, &emsg, &eline, &ecolumn))
    {
        reportError("Could not parse XML file :" + file.fileName() + " because: " + emsg + "");
        return QDomDocument();
    }

    return retval;
}

void XmlFileHandler::writeXmlFile(QDomDocument& doc, QString& filename)
{
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QTextStream stream(&file);
        stream << doc.toString(4);
        file.close();
    }
    else
    {
        reportError("Could not open " + file.fileName() + " Error: " + file.errorString());
    }
}



} // namespace cx
