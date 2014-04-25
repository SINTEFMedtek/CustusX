// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

/*
 * cxMetricWidget.h
 *
 *  \date Jul 5, 2011
 *      \author christiana
 */

#ifndef CXMETRICWIDGET_H_
#define CXMETRICWIDGET_H_

#include "cxBaseWidget.h"

#include <vector>
#include <QtGui>
#include "cxForwardDeclarations.h"
#include "cxLandmark.h"
#include "cxPointMetric.h"
#include "cxDistanceMetric.h"
#include "cxAngleMetric.h"
#include "cxPlaneMetric.h"
#include "cxStringDataAdapterXml.h"
#include "cxVector3DDataAdapterXml.h"
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
class CXNoBorderToolButton : public QToolButton
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
class MetricWidget : public BaseWidget
{
  Q_OBJECT

public:
  MetricWidget(QWidget* parent);
  virtual ~MetricWidget();

  virtual QString defaultWhatsThis() const;

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

private:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QHideEvent* event);
  void enablebuttons();
  MetricBasePtr createMetricWrapper(DataPtr data);
  std::vector<MetricBasePtr> createMetricWrappers();
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
