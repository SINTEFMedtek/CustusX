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

/*
 * cxMetricWidget.h
 *
 *  \date Jul 5, 2011
 *      \author christiana
 */

#ifndef CXMETRICWIDGET_H_
#define CXMETRICWIDGET_H_

#include "cxGuiExport.h"

#include "cxBaseWidget.h"

#include <vector>
#include <QtWidgets>

#include "cxForwardDeclarations.h"
#include "cxLandmark.h"
#include "cxPointMetric.h"
#include "cxDistanceMetric.h"
#include "cxAngleMetric.h"
#include "cxPlaneMetric.h"
#include "cxStringProperty.h"
#include "cxVector3DProperty.h"
#include "cxDataMetricWrappers.h"
#include "cxFrameMetric.h"
#include "cxLegacySingletons.h"

class QVBoxLayout;
class QTableWidget;
class QPushButton;

/** QToolButton descendant with dedicated style sheet: no border
  *
  * \date 2014-04-25
  * \author Christian Askeland, SINTEF
  */
class cxGui_EXPORT CXNoBorderToolButton : public QToolButton
{
	Q_OBJECT
public:
	CXNoBorderToolButton(QWidget* parent=0) : QToolButton(parent) {}
};

namespace cx
{
typedef boost::shared_ptr<class MetricManager> MetricManagerPtr;


/**
 * \ingroup cx_gui
 *
 */
class cxGui_EXPORT MetricWidget : public BaseWidget
{
  Q_OBJECT

public:
  MetricWidget(ViewServicePtr viewService, PatientModelServicePtr patientModelService, QWidget* parent);
  virtual ~MetricWidget();

signals:

 public slots:
  virtual void setModified();

protected slots:
	void itemSelectionChanged();

  void removeButtonClickedSlot();

  void loadReferencePointsSlot();
  void addPointButtonClickedSlot();
  void addFrameButtonClickedSlot();
  void addToolButtonClickedSlot();
  void addPlaneButtonClickedSlot();
  void addAngleButtonClickedSlot();
  void addDistanceButtonClickedSlot();
  void addSphereButtonClickedSlot();
  void addDonutButtonClickedSlot();

  void cellChangedSlot(int row, int col);
  virtual void cellClickedSlot(int row, int column);
  void exportMetricsButtonClickedSlot();
  void delayedUpdate();

protected:
  QAction* mPointMetricAction;
  QAction* mFrameMetricAction;
  QAction* mToolMetricAction;
  QAction* mExportFramesAction;
  PatientModelServicePtr mPatientModelService;
  ViewServicePtr mViewService;

private:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QHideEvent* event);
  void enablebuttons();
  MetricBasePtr createMetricWrapper(ViewServicePtr viewService, PatientModelServicePtr patientModelService, DataPtr data);
  std::vector<MetricBasePtr> createMetricWrappers(ViewServicePtr viewService, PatientModelServicePtr patientModelService);
  virtual void prePaintEvent();
  std::set<QString> getSelectedUids();
  void createActions(QActionGroup* group);
  bool checkEqual(const std::vector<MetricBasePtr>& a, const std::vector<MetricBasePtr>& b) const;
  void resetWrappersAndEditWidgets(std::vector<MetricBasePtr> wrappers);
  void initializeTable();
  void updateTableContents();
  void expensizeColumnResize();
  void updateMetricWrappers();

  QAction* createAction(QActionGroup* group, QString iconName, QString text, QString tip, const char* slot);

  QVBoxLayout* mVerticalLayout; ///< vertical layout is used
  QTableWidget* mTable; ///< the table widget presenting the landmarks

  std::vector<MetricBasePtr> mMetrics;

  QAction* mRemoveAction; ///< the Remove Landmark button
  QAction* mLoadReferencePointsAction; ///< button for loading a reference tools reference points
  QStackedWidget* mEditWidgets;
  MetricManagerPtr mMetricManager;
  int mModifiedCount;
  int mPaintCount;
  QTimer* mDelayedUpdateTimer;
  bool mLocalModified;
};

}//end namespace cx

#endif /* CXMETRICWIDGET_H_ */
