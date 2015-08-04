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

#ifndef CXUR5WIDGET_H_
#define CXUR5WIDGET_H_

#include <QWidget>

#include "cxUr5Connection.h"
#include "org_custusx_robot_ur5_Export.h"

// From design
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
//




class QVBoxLayout;
class QHBoxLayout;

namespace cx
{

/**
 * Widget for use in the UR5 plugin
 *
 * \ingroup org_custusx_robot_ur5
 *
 * \date 2014-05-02
 * \author Christian Askeland
 */
class org_custusx_robot_ur5_EXPORT Ur5Widget : public QWidget
{
    Q_OBJECT
public:
    Ur5Widget(QWidget* parent = 0);
    virtual ~Ur5Widget();

    // For initalize tab
    QLineEdit *ipLineEdit, *manualCoordinates;
    QPushButton *connectButton, *initializeButton, *initializeButton_2, *disconnectButton;
    QComboBox *presetOrigoComboBox;
    QProgressBar *initializeBar;
    //

    // For manual move tab
    QPushButton *negZButton, *posZButton, *posXButton, *negYButton, *posYButton, *stopMove, *negXButton;
    QScrollBar *xScrollBar, *yScrollBar, *zScrollBar;
    QLineEdit *xPosLineEdit, *yPosLineEdit, *zPosLineEdit;
    QScrollBar *rxScrollBar, *ryScrollBar, *rzScrollBar;
    QLineEdit *rxLineEdit, *ryLineEdit, *rzLineEdit;
    QLineEdit *accelerationLineEdit, *velocityLineEdit, *timeLineEdit;
    //

    void setupUi(QWidget *parent);

    void insertPlannedMoveTab(QTabWidget *parent);
    void insertInitializeTab(QTabWidget *parent);
    void insertManualMoveTab(QTabWidget *parent);

    Ur5Connection connection;

    void test();
protected slots:
    void connectButtonSlot();
    void checkConnection();
    void initializeButtonSlot();
    void posZButtonSlotPushed();
    void negZButtonSlotPushed();
    void posYButtonSlotPushed();
    void negYButtonSlotPushed();
    void posXButtonSlotPushed();
    void negXButtonSlotPushed();
    void moveButtonSlotReleased();
    void disconnectButtonSlot();

private:
    QString defaultWhatsThis() const;
    QVBoxLayout*  mVerticalLayout;
    QHBoxLayout* mHorisontalLayout;

};

} /* namespace cx */


#endif /* CXUR5WIDGET_H_ */
