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


#include "cxUr5Widget.h"
#include "cxUr5Connection.h"

#include "widgets/cxUr5MiscInformation.h"
#include "widgets/cxUr5Initialize.h"
#include "widgets/cxUr5ManualMove.h"
#include "widgets/cxUr5PlannedMove.h"

#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>
#include "cxLogger.h"


namespace cx
{

Ur5Widget::Ur5Widget(QWidget* parent) :
    QWidget(parent)
{
    mUr5Robot = Ur5RobotPtr(new Ur5Robot);

    setupUi(this);
}

Ur5Widget::~Ur5Widget()
{
}

void Ur5Widget::setupUi(QWidget *Ur5Widget)
{
    Ur5Widget->setObjectName("Ur5Widget");
    Ur5Widget->setWindowTitle("UR5 Robot");

    QLayout* Ur5WidgetLayout = new QVBoxLayout(Ur5Widget);
    QTabWidget* tabWidget = new QTabWidget(Ur5Widget);
    Ur5WidgetLayout->addWidget(tabWidget);
    tabWidget->addTab(new Ur5InitializeTab(mUr5Robot), tr("Initialize"));
    tabWidget->addTab(new Ur5ManualMoveTab(mUr5Robot),tr("Manual movement"));
    tabWidget->addTab(new Ur5PlannedMoveTab(mUr5Robot),tr("Planned movement"));
    tabWidget->addTab(new Ur5MiscInformationTab(mUr5Robot),tr("Misc information"));

    QMetaObject::connectSlotsByName(Ur5Widget);
}

} /* namespace cx */
