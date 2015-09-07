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

#include "cxRecordBaseWidget.h"

#include <QPushButton>
#include <QFont>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <vtkPolyData.h>
#include "cxTrackingService.h"

#include "cxMesh.h"
#include "cxTransform3D.h"
#include "cxRecordSessionWidget.h"
#include "cxSoundSpeedConversionWidget.h"

namespace cx
{

//RecordBaseWidget::RecordBaseWidget(AcquisitionServicePtr acquisitionService, QWidget* parent, QString description):
//    BaseWidget(parent, "RecordBaseWidget", "Record Base"),
//	mAcquisitionService(acquisitionService),
//    mLayout(new QVBoxLayout(this))
//{
//	this->setObjectName("RecordBaseWidget");
//	this->setWindowTitle("Record Base");

//	mRecordSessionWidget = new RecordSessionWidget(mAcquisitionService, this, description);
//	mLayout->addWidget(mRecordSessionWidget);
//}

//RecordBaseWidget::~RecordBaseWidget()
//{}

//////----------------------------------------------------------------------------------------------------------------------
//////----------------------------------------------------------------------------------------------------------------------
//////----------------------------------------------------------------------------------------------------------------------

//TrackedRecordWidget::TrackedRecordWidget(AcquisitionServicePtr acquisitionService, QWidget* parent, QString description) :
//  RecordBaseWidget(acquisitionService, parent, description)
//{}

//TrackedRecordWidget::~TrackedRecordWidget()
//{}

//void TrackedRecordWidget::setTool(ToolPtr tool)
//{
//  if(mTool && tool && (mTool->getUid() == tool->getUid()))
//    return;

//  mTool = tool;
//  emit toolChanged();
//}

//ToolPtr TrackedRecordWidget::getTool()
//{
//  return mTool;
//}
////----------------------------------------------------------------------------------------------------------------------
}//namespace cx
