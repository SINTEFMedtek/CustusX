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
#include "sscForwardDeclarations.h"
#include "sscLandmark.h"
#include "sscPointMetric.h"
#include "sscDistanceMetric.h"
#include "sscAngleMetric.h"
#include "sscPlaneMetric.h"
#include "sscStringDataAdapterXml.h"
#include "sscVector3DDataAdapterXml.h"
#include "cxDataMetricWrappers.h"
#include "cxFrameMetric.h"

class QVBoxLayout;
class QTableWidget;
class QPushButton;

namespace cx
{


/**
 * \ingroup cxGUI
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

protected:
  QAction* mPointMetricAction;
  QAction* mFrameMetricAction;
  QAction* mToolMetricAction;
  QAction* mExportFramesAction;

  void exportMetricsToFile(QString filename);

private:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QHideEvent* event);
  void enablebuttons();
  void setActiveUid(QString uid);
  PointMetricPtr addPoint(Vector3D point, CoordinateSystem space=CoordinateSystem(csREF), QString name="point%1");
  MetricBasePtr createMetricWrapper(DataPtr data);
  std::vector<MetricBasePtr> createMetricWrappers();
  std::vector<DataPtr> refinePointArguments(std::vector<DataPtr> args, unsigned argNo);
  void setManualToolPosition(Vector3D p_r);
  void prePaintEvent();
  std::vector<DataPtr> getSpecifiedNumberOfValidArguments(MetricReferenceArgumentListPtr arguments, int numberOfRequiredArguments=-1);
  void installNewMetric(DataMetricPtr metric);
  PointMetricPtr addPointInDefaultPosition();

//  template<class T>
  QAction* createAction(QActionGroup* group, QString iconName, QString text, QString tip, const char* slot);

  QVBoxLayout* mVerticalLayout; ///< vertical layout is used
  QTableWidget* mTable; ///< the table widget presenting the landmarks
  QString mActiveLandmark; ///< uid of surrently selected landmark.

  std::vector<MetricBasePtr> mMetrics;

  QAction* mRemoveAction; ///< the Remove Landmark button
  QAction* mLoadReferencePointsAction; ///< button for loading a reference tools reference points
  QStackedWidget* mEditWidgets;
};

}//end namespace cx

#endif /* CXMETRICWIDGET_H_ */
