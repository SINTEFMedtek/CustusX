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

#ifndef CXFRAMETREEWIDGET_H_
#define CXFRAMETREEWIDGET_H_

#include "cxResourceWidgetsExport.h"

#include "cxBaseWidget.h"

#include <map>
#include <string>
#include <QWidget>
#include "cxForwardDeclarations.h"
class QTreeWidget;
class QTreeWidgetItem;
class QDomNode;

namespace cx
{

/**
 * \class FrameTreeWidget
 *
 *\brief Widget for displaying the FrameForest object
 * \ingroup cx_gui
 *
 *\date Sep 23, 2010
 *\\author Christian Askeland, SINTEF
 */
class cxResourceWidgets_EXPORT FrameTreeWidget : public BaseWidget
{
  Q_OBJECT
public:
  FrameTreeWidget(PatientModelServicePtr patientService, QWidget* parent);
  ~FrameTreeWidget() {}

protected:
  virtual void prePaintEvent();
private:
  PatientModelServicePtr mPatientService;
  QTreeWidget* mTreeWidget;
  void fill(QTreeWidgetItem* parent, QDomNode node);
  std::map<QString, DataPtr> mConnectedData;

private slots:
  void dataLoadedSlot();
  void rebuild(); // TODO this must also listen to all changed() in all data
};


}

#endif /* CXFRAMETREEWIDGET_H_ */
