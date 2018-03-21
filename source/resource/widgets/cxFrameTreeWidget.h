/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
