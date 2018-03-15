/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXTRACKER_H
#define CXTRACKER_H

#include "cxResourceExport.h"

#include <QObject>
#include <QStringList>

namespace cx {

class cxResource_EXPORT Tracker : public QObject
{
    Q_OBJECT
public:
    explicit Tracker(QObject *parent = 0);

    static QStringList getSupportedTrackingSystems();

};
}//namespace cx

#endif // CXTRACKER_H
