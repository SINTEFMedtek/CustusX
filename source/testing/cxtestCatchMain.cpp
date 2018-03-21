/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

