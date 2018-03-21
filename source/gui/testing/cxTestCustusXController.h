/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
