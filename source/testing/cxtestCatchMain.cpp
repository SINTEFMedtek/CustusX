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

#include "cxtestCatchImpl.h"

#include "cxImportTests.h"

#ifdef CX_WINDOWS
#include <windows.h>
#endif

#include <QLibrary>
#include <QString>
#include <QStringList>

void load_plugin(std::string path)
{
    QString libPath(path.c_str());
    QLibrary library(libPath);
    bool loaded = library.load();

    if(!loaded)
        printf("%s library failed to load!\n", path.c_str());
}

void load_plugins()
{
    QString str = QString(CX_SHARED_TEST_LIBRARIES);
    QStringList list = str.split(";");

    std::vector<std::string> plugins;
    foreach(const QString &item, list)
    {
        plugins.push_back(item.toStdString());
    }

    std::vector<std::string>::iterator it = plugins.begin();
    for(; it != plugins.end(); it++)
        load_plugin(*it);

}

int main(int argc, char *argv[])
{

#ifdef CX_WINDOWS
	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
#endif

    load_plugins();
    int error_code = cxtest::CatchImpl().run(argc, argv);

    return error_code;
}

