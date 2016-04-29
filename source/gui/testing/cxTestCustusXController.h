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
#ifndef CXTESTCUSTUSXCONTROLLER_H_
#define CXTESTCUSTUSXCONTROLLER_H_

#include "cxtestgui_export.h"

#include <QApplication>
#include "cxMainWindow.h"
#include "cxMainWindowApplicationComponent.h"

/**Helper object for automated control of the CustusX application.
 *
 */
class CXTESTGUI_EXPORT CustusXController : public QObject
{
  Q_OBJECT

public:
//    cx::MainWindow* mMainWindow;
    QString mPatientFolder;
    QString mTestData;
    int mBaseTime;
    double mMeasuredFPS;
    bool mEnableSlicing;

    CustusXController(QObject* parent);
  ~CustusXController();
    void start();
    void stop();

public slots:
    void loadPatientSlot();
    void enableSlicingSlot();
    void initialBeginCheckRenderSlot();
    void initialEndCheckRenderSlot();
    void secondEndCheckRenderSlot();
    void displayResultsSlot();
    void changeToNewProfile();

private:
	typedef cx::MainWindowApplicationComponent<cx::MainWindow> MainWindowComponent;
	boost::shared_ptr<MainWindowComponent> mApplicationComponent;

    int mNumInitialRenders;
};

#endif /* CXTESTCUSTUSXCONTROLLER_H_ */
