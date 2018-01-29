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
