/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
  MetricWidget(VisServicesPtr services, QWidget* parent);
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
  void addROIButtonClickedSlot();
  void addSphereButtonClickedSlot();
  void addDonutButtonClickedSlot();
  void addCustomButtonClickedSlot();

  void cellChangedSlot(int row, int col);
  virtual void cellClickedSlot(int row, int column);
  void exportMetricsButtonClickedSlot();
  void importMetricsButtonClickedSlot();
  void delayedUpdate();

protected:
  QAction* mPointMetricAction;
  QAction* mFrameMetricAction;
  QAction* mToolMetricAction;
  QAction* mExportFramesAction;
  QAction* mImportFramesAction;
  VisServicesPtr mServices;

private:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QHideEvent* event);
  void enablebuttons();
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
